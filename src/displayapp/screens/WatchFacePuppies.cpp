#include "displayapp/screens/WatchFacePuppies.h"

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

WatchFacePuppies::WatchFacePuppies(Controllers::DateTime& dateTimeController,
                               const Controllers::Battery& batteryController,
                               const Controllers::Ble& bleController,
                               const Controllers::AlarmController& alarmController,
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
    bleController{bleController},
    statusIcons(batteryController, bleController, alarmController){

  // Create the rolling hills background first
  createRollingHillsBackground();

  statusIcons.Create();

  // Center time display (moved down by 20px)
  label_time = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_font(label_time, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_extrabold_compressed);
  lv_obj_set_style_local_text_color(label_time, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x2E4B2E)); // Dark green
  lv_obj_align(label_time, lv_scr_act(), LV_ALIGN_CENTER, 0, -30);

  // AM/PM indicator below time
  label_time_ampm = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_text_static(label_time_ampm, "");
  lv_obj_set_style_local_text_color(label_time_ampm, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x2E4B2E)); // Dark green
  lv_obj_align(label_time_ampm, label_time, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);

  // Date above time (moved down by 20px)
  label_date = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(label_date, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x2E4B2E)); // Dark green
  lv_obj_set_style_local_text_font(label_date, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_bold_20);
  lv_obj_align(label_date, lv_scr_act(), LV_ALIGN_CENTER, 0, -70);
  
  // BLE icon next to date
  bleIcon = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(bleIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x2E4B2E));
  lv_label_set_text_static(bleIcon, Symbols::bluetooth);
  lv_obj_align(bleIcon, label_date, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

  // Battery indicator in top right
  //batteryIcon = lv_label_create(lv_scr_act(), nullptr);
  //lv_obj_set_style_local_text_color(batteryIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x2E4B2E)); // Dark green
  //lv_label_set_text_static(batteryIcon, Symbols::batteryFull);
  //lv_obj_align(batteryIcon, lv_scr_act(), LV_ALIGN_IN_TOP_RIGHT, 0, 10);

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

  // Notification indicator at bottom left (above hills)
  notificationIcon = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(notificationIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x8B4513)); // Saddle brown
  lv_label_set_text_static(notificationIcon, NotificationIcon::GetIcon(false));
  lv_obj_align(notificationIcon, lv_scr_act(), LV_ALIGN_IN_BOTTOM_LEFT, 10, -90);

  // Create Wilson the dog image (repositioned)
  wilsonImg = lv_img_create(lv_scr_act(), nullptr);
  lv_img_set_src(wilsonImg, "F:/images/wilson_day.bin");
  lv_obj_align(wilsonImg, lv_scr_act(), LV_ALIGN_IN_BOTTOM_MID, 0, -15);
  lv_obj_set_click(wilsonImg, false);

  taskRefresh = lv_task_create(RefreshTaskCallback, LV_DISP_DEF_REFR_PERIOD, LV_TASK_PRIO_MID, this);
  Refresh();
}

WatchFacePuppies::~WatchFacePuppies() {
  lv_task_del(taskRefresh);
  lv_obj_clean(lv_scr_act());
}

void WatchFacePuppies::createRollingHillsBackground() {
  // ... (no changes in this function)
  // Create main background
  lv_obj_t* bg = lv_obj_create(lv_scr_act(), nullptr);
  lv_obj_set_size(bg, 240, 240);
  lv_obj_set_pos(bg, 0, 0);
  lv_obj_set_style_local_bg_color(bg, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xB8E6B8)); // Light green base color
  lv_obj_set_style_local_border_width(bg, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_click(bg, false);

  // Create gradient sky (top 160px)
  sky = lv_obj_create(bg, nullptr);
  lv_obj_set_size(sky, 240, 160);
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

  // Create rolling hills with circles
  // Back hills (lighter green)
  hills[0] = lv_obj_create(lv_scr_act(), nullptr);
  lv_obj_set_size(hills[0], 200, 120);
  lv_obj_set_pos(hills[0], -30, 140);
  lv_obj_set_style_local_bg_color(hills[0], LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xA8D5A8)); // Light green
  lv_obj_set_style_local_border_width(hills[0], LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_style_local_radius(hills[0], LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
  lv_obj_set_click(hills[0], false);
  
  hills[1] = lv_obj_create(lv_scr_act(), nullptr);
  lv_obj_set_size(hills[1], 180, 100);
  lv_obj_set_pos(hills[1], 70, 160);
  lv_obj_set_style_local_bg_color(hills[1], LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xA8D5A8)); // Light green
  lv_obj_set_style_local_border_width(hills[1], LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_style_local_radius(hills[1], LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
  lv_obj_set_click(hills[1], false);
  
  // Middle hills (medium green)
  hills[2] = lv_obj_create(lv_scr_act(), nullptr);
  lv_obj_set_size(hills[2], 150, 90);
  lv_obj_set_pos(hills[2], -20, 170);
  lv_obj_set_style_local_bg_color(hills[2], LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x7CB57C)); // Medium green
  lv_obj_set_style_local_border_width(hills[2], LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_style_local_radius(hills[2], LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
  lv_obj_set_click(hills[2], false);
  
  hills[3] = lv_obj_create(lv_scr_act(), nullptr);
  lv_obj_set_size(hills[3], 160, 95);
  lv_obj_set_pos(hills[3], 100, 175);
  lv_obj_set_style_local_bg_color(hills[3], LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x7CB57C)); // Medium green
  lv_obj_set_style_local_border_width(hills[3], LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_style_local_radius(hills[3], LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
  lv_obj_set_click(hills[3], false);
  
  // Front hills (darker green)
  hills[4] = lv_obj_create(lv_scr_act(), nullptr);
  lv_obj_set_size(hills[4], 140, 80);
  lv_obj_set_pos(hills[4], 30, 190);
  lv_obj_set_style_local_bg_color(hills[4], LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x5A9A5A)); // Darker green
  lv_obj_set_style_local_border_width(hills[4], LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_style_local_radius(hills[4], LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
  lv_obj_set_click(hills[4], false);
  
  hills[5] = lv_obj_create(lv_scr_act(), nullptr);
  lv_obj_set_size(hills[5], 120, 70);
  lv_obj_set_pos(hills[5], 140, 195);
  lv_obj_set_style_local_bg_color(hills[5], LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x5A9A5A)); // Darker green
  lv_obj_set_style_local_border_width(hills[5], LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_style_local_radius(hills[5], LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
  lv_obj_set_click(hills[5], false);
  
  // Move the background to the back
  lv_obj_move_background(bg);
}

// Function to update sky based on weather and time (reused from Shire)
void WatchFacePuppies::updateSkyForWeatherAndTime() {
  // ... (no changes in this function)
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
    lv_obj_set_style_local_text_color(bleIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xe4d3a0));        // Parchment
    lv_obj_set_style_local_text_color(batteryIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xe4d3a0));   // Parchment
  }

  // Update sun/moon based on time
  if (isDaytime) {
    lv_obj_set_style_local_bg_color(sun, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFD700)); // Gold
  } else {
    lv_obj_set_style_local_bg_color(sun, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xF0F0F0)); // Light silver (moon)
  }
}

// Function to update Wilson image based on time, weather, and temperature
void WatchFacePuppies::updateWilsonImage() {
  // ... (no changes in this function)
  // Get current time, weather, and temperature
  uint8_t hour = dateTimeController.Hours();
  auto weather = weatherService.Current();
  
  // Determine if it's day or night
  bool isDaytime = (hour >= 6 && hour < 18);

  // NEW LOGIC: Check for night first, as it has the highest priority
  if (!isDaytime) {
    lv_img_set_src(wilsonImg, "F:/images/wilson_sleep.bin");
    return; // No need to check weather if it's night
  }

  // It's daytime, now check weather and temperature
  // Check if it's cold (below 40°F)
  bool isCold = false;
  if (weather) {
    int16_t tempF = weather->temperature.Fahrenheit();
    isCold = (tempF < 40);
  }
  
  // Check if it's raining
  bool isRaining = false;
  if (weather) {
    isRaining = (weather->iconId == static_cast<Pinetime::Controllers::SimpleWeatherService::Icons>(4) ||
                 weather->iconId == static_cast<Pinetime::Controllers::SimpleWeatherService::Icons>(5));
  }
  
  // Determine which daytime image to use
  // Priority: Rain > Cold > Default Day
  if (isRaining) {
    lv_img_set_src(wilsonImg, "F:/images/wilson_raining.bin");
  } else if (isCold) {
    lv_img_set_src(wilsonImg, "F:/images/wilson_cold.bin");
  } else {
    lv_img_set_src(wilsonImg, "F:/images/wilson_day.bin");
  }
}

void WatchFacePuppies::Refresh() {
  statusIcons.Update();

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
      lv_obj_realign(bleIcon);
    }

    bleState = bleController.IsConnected();
    bleRadioEnabled = bleController.IsRadioEnabled();
    if (bleState.IsUpdated()) {
      lv_label_set_text_static(bleIcon, BleIcon::GetIcon(bleState.Get()));
    }

    
    
    updateSkyForWeatherAndTime();
    updateWilsonImage();
  }

  // Heart rate display logic removed
  heartbeat = heartRateController.HeartRate();
  heartbeatRunning = heartRateController.State() != Controllers::HeartRateController::States::Stopped;
  // This block is now empty, but we keep the checks to prevent unnecessary updates

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
    
    // Update Wilson image when weather changes
    updateWilsonImage();
  }
}

bool WatchFacePuppies::IsAvailable(Pinetime::Controllers::FS& filesystem) {
  lfs_file file = {};

  if (filesystem.FileOpen(&file, "/images/wilson_sleep.bin", LFS_O_RDONLY) < 0) {
    return false;
  }

  filesystem.FileClose(&file);
  return true;
}