#include "displayapp/screens/WatchFaceShire.h"

#include <lvgl/lvgl.h>
#include <cstdio>

#include "displayapp/screens/NotificationIcon.h"
#include "displayapp/screens/Symbols.h"
#include "displayapp/screens/WeatherSymbols.h"
#include "components/battery/BatteryController.h"
#include "components/ble/BleController.h"
#include "displayapp/screens/BleIcon.h"
#include "components/ble/NotificationManager.h"
#include "components/heartrate/HeartRateController.h"
#include "components/motion/MotionController.h"
#include "components/ble/SimpleWeatherService.h"
#include "components/settings/Settings.h"

using namespace Pinetime::Applications::Screens;

WatchFaceShire::WatchFaceShire(Controllers::DateTime& dateTimeController,
                               const Controllers::Battery& batteryController,
                               const Controllers::Ble& bleController,
                               Controllers::NotificationManager& notificationManager,
                               Controllers::Settings& settingsController,
                               Controllers::HeartRateController& heartRateController,
                               Controllers::MotionController& motionController,
                               Controllers::SimpleWeatherService& weatherService)
  : currentDateTime{{}},
    dateTimeController{dateTimeController},
    notificationManager{notificationManager},
    settingsController{settingsController},
    heartRateController{heartRateController},
    motionController{motionController},
  weatherService{weatherService},
  batteryController{batteryController},
  bleController{bleController}{

  // Create the hobbit hole background first
  createHobbitHoleBackground();

  //statusIcons.Create();

  // Center time display (in the upper area, above the hobbit hole)
  label_time = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_font(label_time, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_extrabold_compressed);
  lv_obj_set_style_local_text_color(label_time, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x2E4B2E)); // Dark green
  lv_obj_align(label_time, lv_scr_act(), LV_ALIGN_CENTER, 0, -30);

  // AM/PM indicator below time
  label_time_ampm = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_text_static(label_time_ampm, "");
  lv_obj_set_style_local_text_color(label_time_ampm, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x2E4B2E)); // Dark green
  lv_obj_align(label_time_ampm, label_time, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);

  // Date above time
  label_date = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(label_date, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x2E4B2E)); // Dark green
  lv_obj_set_style_local_text_font(label_date, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_bold_20);
  lv_obj_align(label_date, lv_scr_act(), LV_ALIGN_CENTER, 0, -70);

  // Heart rate at left side
  heartbeatIcon = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_text_static(heartbeatIcon, Symbols::heartBeat);
  lv_obj_set_style_local_text_color(heartbeatIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x8B4513)); // Saddle brown
  lv_obj_align(heartbeatIcon, lv_scr_act(), LV_ALIGN_IN_BOTTOM_LEFT, 10, 0);

  heartbeatValue = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(heartbeatValue, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x8B4513)); // Saddle brown
  lv_label_set_text_static(heartbeatValue, "");
  lv_obj_align(heartbeatValue, heartbeatIcon, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

  // Steps at right side
  stepIcon = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(stepIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x8B4513)); // Saddle brown
  lv_label_set_text_static(stepIcon, Symbols::shoe);
  lv_obj_align(stepIcon, lv_scr_act(), LV_ALIGN_IN_BOTTOM_RIGHT, -10, 0);

  stepValue = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(stepValue, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x8B4513)); // Saddle brown
  lv_label_set_text_static(stepValue, "0");
  lv_obj_align(stepValue, stepIcon, LV_ALIGN_OUT_LEFT_MID, -5, 0);

  // Weather in top left
  weatherIcon = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(weatherIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x2E4B2E)); // Dark green
  lv_obj_set_style_local_text_font(weatherIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &fontawesome_weathericons);
  lv_label_set_text(weatherIcon, "");
  lv_obj_align(weatherIcon, lv_scr_act(), LV_ALIGN_IN_TOP_LEFT, 0, 10);

  temperature = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(temperature, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x2E4B2E)); // Dark green
  lv_label_set_text(temperature, "");
  lv_obj_align(temperature, weatherIcon, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

  // Notification indicator at bottom left (above hobbit hole)
  notificationIcon = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(notificationIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x8B4513)); // Saddle brown
  lv_label_set_text_static(notificationIcon, NotificationIcon::GetIcon(false));
  lv_obj_align(notificationIcon, lv_scr_act(), LV_ALIGN_IN_BOTTOM_LEFT, 10, -90);

  bleIcon = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(bleIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x2E4B2E));
  lv_label_set_text_static(bleIcon, Symbols::bluetooth);
  //lv_obj_align(bleIcon, dateContainer, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

  taskRefresh = lv_task_create(RefreshTaskCallback, LV_DISP_DEF_REFR_PERIOD, LV_TASK_PRIO_MID, this);
  Refresh();
}

WatchFaceShire::~WatchFaceShire() {
  lv_task_del(taskRefresh);
  lv_obj_clean(lv_scr_act());
}

void WatchFaceShire::createHobbitHoleBackground() {
  // Create main background
  lv_obj_t* bg = lv_obj_create(lv_scr_act(), nullptr);
  lv_obj_set_size(bg, 240, 240);
  lv_obj_set_pos(bg, 0, 0);

  // Create gradient sky (top 160px)
  sky = lv_obj_create(bg, nullptr);
  lv_obj_set_size(sky, 240, 240);
  lv_obj_set_pos(sky, 0, 0);

  // Create gradient effect for sky
  lv_obj_set_style_local_bg_color(sky, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x87CEEB));      // Sky blue at top
  lv_obj_set_style_local_bg_grad_color(sky, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xB8E6B8)); // Light green at bottom
  lv_obj_set_style_local_bg_grad_dir(sky, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
  lv_obj_set_style_local_border_width(sky, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_click(sky, false);

  // Create sun
  sun = lv_obj_create(sky, nullptr);
  lv_obj_set_size(sun, 30, 30);
  lv_obj_set_pos(sun, 175, 10);
  lv_obj_set_style_local_bg_color(sun, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFD700)); // Gold
  lv_obj_set_style_local_border_width(sun, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_style_local_radius(sun, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
  lv_obj_set_click(sun, false);

  // Create hobbit hole image (bottom 80px)
  shireImg = lv_img_create(lv_scr_act(), nullptr);
  lv_img_set_src(shireImg, "F:/images/shire_img.bin");
  lv_obj_set_pos(shireImg, 0, 160);
  lv_obj_set_click(shireImg, false);

  // Move the background to the back
  lv_obj_move_background(bg);
}

// Function to update sky based on weather and time
void WatchFaceShire::updateSkyForWeatherAndTime() {
  // Get current time and weather
  uint8_t hour = dateTimeController.Hours();
  auto weather = weatherService.Current();

  // Determine if it's day or night
  bool isDaytime = (hour >= 6 && hour < 18);

  // Update sky color based on time and weather
  if (isDaytime) {
    if (weather && weather->iconId == static_cast<Pinetime::Controllers::SimpleWeatherService::Icons>(1)) {  // Sunny
      lv_obj_set_style_local_bg_color(sky, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x87CEEB));      // Sky blue
      lv_obj_set_style_local_bg_grad_color(sky, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xB8E6B8)); // Light green
    } else if (weather && (weather->iconId == static_cast<Pinetime::Controllers::SimpleWeatherService::Icons>(2) ||
                           weather->iconId == static_cast<Pinetime::Controllers::SimpleWeatherService::Icons>(3))) { // Cloudy
      lv_obj_set_style_local_bg_color(sky, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xD3D3D3));              // Light gray
      lv_obj_set_style_local_bg_grad_color(sky, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xA9A9A9));         // Dark gray
    } else if (weather && (weather->iconId == static_cast<Pinetime::Controllers::SimpleWeatherService::Icons>(4) ||
                           weather->iconId == static_cast<Pinetime::Controllers::SimpleWeatherService::Icons>(5))) { // Rainy
      lv_obj_set_style_local_bg_color(sky, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x708090));              // Slate gray
      lv_obj_set_style_local_bg_grad_color(sky, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x2F4F4F));         // Dark slate gray
    } else {                                                                                                         // Default daytime
      lv_obj_set_style_local_bg_color(sky, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x87CEEB));              // Sky blue
      lv_obj_set_style_local_bg_grad_color(sky, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xB8E6B8));         // Light green
    }
  } else {                                                                                                 // Nighttime
    lv_obj_set_style_local_bg_color(sky, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x191970));      // Midnight blue
    lv_obj_set_style_local_bg_grad_color(sky, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x000080)); // Navy

    // Set Night
    lv_obj_set_style_local_text_color(label_time, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xe4d3a0));      // Parchment
    lv_obj_set_style_local_text_color(label_time_ampm, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xe4d3a0)); // Parchment
    lv_obj_set_style_local_text_color(label_date, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xe4d3a0));      // Parchment
    lv_obj_set_style_local_text_color(weatherIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xe4d3a0));     // Parchment
    lv_obj_set_style_local_text_color(temperature, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xe4d3a0));     // Parchment
  }

  // Update sun/moon based on time
  if (isDaytime) {
    lv_obj_set_style_local_bg_color(sun, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFD700)); // Gold
  } else {
    lv_obj_set_style_local_bg_color(sun, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xF0F0F0)); // Light silver (moon)
  }
}

void WatchFaceShire::Refresh() {
  //statusIcons.Update();

  notificationState = notificationManager.AreNewNotificationsAvailable();
  if (notificationState.IsUpdated()) {
    lv_label_set_text_static(notificationIcon, NotificationIcon::GetIcon(notificationState.Get()));
  }

  currentDateTime = std::chrono::time_point_cast<std::chrono::minutes>(dateTimeController.CurrentDateTime());

  if (currentDateTime.IsUpdated()) {
    uint8_t hour = dateTimeController.Hours();
    uint8_t minute = dateTimeController.Minutes();

    if (settingsController.GetClockType() == Controllers::Settings::ClockType::H12) {
      char ampmChar[3] = "AM";
      if (hour == 0) {
        hour = 12;
      } else if (hour == 12) {
        ampmChar[0] = 'P';
      } else if (hour > 12) {
        hour = hour - 12;
        ampmChar[0] = 'P';
      }
      lv_label_set_text(label_time_ampm, ampmChar);
      lv_label_set_text_fmt(label_time, "%2d:%02d", hour, minute);
    } else {
      lv_label_set_text_fmt(label_time, "%02d:%02d", hour, minute);
      lv_label_set_text_static(label_time_ampm, "");
    }
    lv_obj_realign(label_time);
    lv_obj_realign(label_time_ampm);

    currentDate = std::chrono::time_point_cast<std::chrono::days>(currentDateTime.Get());
    if (currentDate.IsUpdated()) {
      uint16_t year = dateTimeController.Year();
      uint8_t day = dateTimeController.Day();
      const char* dayOfWeek = dateTimeController.DayOfWeekShortToString();
      const char* month = dateTimeController.MonthShortToString();

      if (settingsController.GetClockType() == Controllers::Settings::ClockType::H24) {
        lv_label_set_text_fmt(label_date, "%s %d %s %d", dayOfWeek, day, month, year);
      } else {
        lv_label_set_text_fmt(label_date, "%s %s %d %d", dayOfWeek, month, day, year);
      }
      lv_obj_realign(label_date);
    }

    bleState = bleController.IsConnected();
    bleRadioEnabled = bleController.IsRadioEnabled();
    if (bleState.IsUpdated()) {
      lv_label_set_text_static(bleIcon, BleIcon::GetIcon(bleState.Get()));
    }

    batteryPercentRemaining = batteryController.PercentRemaining();
    isCharging = batteryController.IsCharging();
    
    updateSkyForWeatherAndTime();
  }

  heartbeat = heartRateController.HeartRate();
  heartbeatRunning = heartRateController.State() != Controllers::HeartRateController::States::Stopped;
  if (heartbeat.IsUpdated() || heartbeatRunning.IsUpdated()) {
    if (heartbeatRunning.Get()) {
      lv_obj_set_style_local_text_color(heartbeatIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x8B4513));
      lv_label_set_text_fmt(heartbeatValue, "%d", heartbeat.Get());
    } else {
      lv_obj_set_style_local_text_color(heartbeatIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x404040));
      lv_label_set_text_static(heartbeatValue, "");
    }

    lv_obj_realign(heartbeatIcon);
    lv_obj_realign(heartbeatValue);
  }

  stepCount = motionController.NbSteps();
  if (stepCount.IsUpdated()) {
    lv_label_set_text_fmt(stepValue, "%lu", stepCount.Get());
    lv_obj_realign(stepValue);
    lv_obj_realign(stepIcon);
  }

  currentWeather = weatherService.Current();
  if (currentWeather.IsUpdated()) {
    auto optCurrentWeather = currentWeather.Get();
    if (optCurrentWeather) {
      int16_t temp = optCurrentWeather->temperature.Celsius();
      char tempUnit = 'C';
      if (settingsController.GetWeatherFormat() == Controllers::Settings::WeatherFormat::Imperial) {
        temp = optCurrentWeather->temperature.Fahrenheit();
        tempUnit = 'F';
      }
      lv_label_set_text_fmt(temperature, "%d°%c", temp, tempUnit);
      lv_label_set_text(weatherIcon, Symbols::GetSymbol(optCurrentWeather->iconId));
    } else {
      lv_label_set_text_static(temperature, "");
      lv_label_set_text(weatherIcon, "");
    }
    lv_obj_realign(temperature);
    lv_obj_realign(weatherIcon);
  }
}

bool WatchFaceShire::IsAvailable(Pinetime::Controllers::FS& filesystem) {
  lfs_file file = {};

  if (filesystem.FileOpen(&file, "/images/shire_img.bin", LFS_O_RDONLY) < 0) {
    return false;
  }

  filesystem.FileClose(&file);
  return true;
}