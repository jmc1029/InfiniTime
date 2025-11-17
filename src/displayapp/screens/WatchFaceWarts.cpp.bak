#include "displayapp/screens/WatchFaceWarts.h"

#include <lvgl/lvgl.h>
#include <cstdio>

#include "displayapp/screens/NotificationIcon.h"
#include "displayapp/screens/Symbols.h"
#include "displayapp/screens/WeatherSymbols.h"
#include "components/battery/BatteryController.h"
#include "components/ble/BleController.h"
#include "components/ble/NotificationManager.h"
#include "components/heartrate/HeartRateController.h"
#include "components/motion/MotionController.h"
#include "components/ble/SimpleWeatherService.h"
#include "components/settings/Settings.h"

using namespace Pinetime::Applications::Screens;

WatchFaceWarts::WatchFaceWarts(Controllers::DateTime& dateTimeController,
                                   const Controllers::Battery& batteryController,
                                   const Controllers::Ble& bleController,
                                   Controllers::NotificationManager& notificationManager,
                                   Controllers::Settings& settingsController,
                                   Controllers::HeartRateController& heartRateController,
                                   Controllers::MotionController& motionController,
                                   Controllers::SimpleWeatherService& weatherService)
  : currentDateTime {{}},
    dateTimeController {dateTimeController},
    notificationManager {notificationManager},
    settingsController {settingsController},
    heartRateController {heartRateController},
    motionController {motionController},
    weatherService {weatherService},
    statusIcons(batteryController, bleController),
    currentHouseTheme {HouseTheme::Lion} { // Default to Lion (Gryffindor)

  // Create the Hogwarts background first
  createHogwartsBackground();

  statusIcons.Create();

  // Center time display
  label_time = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_font(label_time, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_extrabold_compressed);
  lv_obj_set_style_local_text_color(label_time, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF)); // White
  lv_obj_align(label_time, lv_scr_act(), LV_ALIGN_CENTER, 0, -20);

  // AM/PM indicator below time
  label_time_ampm = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_text_static(label_time_ampm, "");
  lv_obj_set_style_local_text_color(label_time_ampm, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF)); // White
  lv_obj_align(label_time_ampm, label_time, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);

  // Date above time
  label_date = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(label_date, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF)); // White
  lv_obj_set_style_local_text_font(label_date, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_bold_20);
  lv_obj_align(label_date, lv_scr_act(), LV_ALIGN_CENTER, 0, -60);

  // Heart rate at left side
  heartbeatIcon = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_text_static(heartbeatIcon, Symbols::heartBeat);
  lv_obj_set_style_local_text_color(heartbeatIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF)); // White
  lv_obj_align(heartbeatIcon, lv_scr_act(), LV_ALIGN_IN_TOP_LEFT, 10, 10);

  heartbeatValue = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(heartbeatValue, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF)); // White
  lv_label_set_text_static(heartbeatValue, "");
  lv_obj_align(heartbeatValue, heartbeatIcon, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

  // Steps at right side
  stepIcon = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(stepIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF)); // White
  lv_label_set_text_static(stepIcon, Symbols::shoe);
  lv_obj_align(stepIcon, lv_scr_act(), LV_ALIGN_IN_TOP_RIGHT, -10, 10);

  stepValue = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(stepValue, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF)); // White
  lv_label_set_text_static(stepValue, "0");
  lv_obj_align(stepValue, stepIcon, LV_ALIGN_OUT_LEFT_MID, -5, 0);

  // Weather in top center
  weatherIcon = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(weatherIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF)); // White
  lv_obj_set_style_local_text_font(weatherIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &fontawesome_weathericons);
  lv_label_set_text(weatherIcon, "");
  lv_obj_align(weatherIcon, lv_scr_act(), LV_ALIGN_IN_TOP_MID, 0, 10);

  temperature = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(temperature, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF)); // White
  lv_label_set_text(temperature, "");
  lv_obj_align(temperature, weatherIcon, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

  // Notification icon (added this missing initialization)
  notificationIcon = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(notificationIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF)); // White
  lv_label_set_text_static(notificationIcon, NotificationIcon::GetIcon(false));
  lv_obj_align(notificationIcon, lv_scr_act(), LV_ALIGN_IN_BOTTOM_LEFT, 10, -90);

  // House theme label
  houseLabel = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(houseLabel, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF)); // White
  lv_label_set_text_static(houseLabel, getHouseName());
  lv_obj_align(houseLabel, lv_scr_act(), LV_ALIGN_IN_BOTTOM_LEFT, 10, -10);

  // House theme button (click to cycle through themes)
  houseButton = lv_btn_create(lv_scr_act(), nullptr);
  lv_obj_set_size(houseButton, 80, 30);
  lv_obj_set_pos(houseButton, 150, 200);
  lv_obj_set_style_local_bg_color(houseButton, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, getPrimaryColor());
  lv_obj_set_style_local_border_width(houseButton, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 2);
  lv_obj_set_style_local_border_color(houseButton, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));
  lv_obj_set_style_local_radius(houseButton, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 5);
  lv_obj_set_event_cb(houseButton, [](lv_obj_t* obj, lv_event_t event) {
    if (event == LV_EVENT_CLICKED) {
      auto* screen = static_cast<WatchFaceWarts*>(obj->user_data);
      screen->cycleHouseTheme();
    }
  });
  houseButton->user_data = this;

  taskRefresh = lv_task_create(RefreshTaskCallback, LV_DISP_DEF_REFR_PERIOD, LV_TASK_PRIO_MID, this);
  Refresh();
}

WatchFaceWarts::~WatchFaceWarts() {
  lv_task_del(taskRefresh);
  lv_obj_clean(lv_scr_act());
}

void WatchFaceWarts::createHogwartsBackground() {
  // Create main background
  lv_obj_t * bg = lv_obj_create(lv_scr_act(), nullptr);
  lv_obj_set_size(bg, 240, 240);
  lv_obj_set_pos(bg, 0, 0);
  lv_obj_set_style_local_bg_color(bg, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x0A0A0A)); // Dark background
  lv_obj_set_style_local_border_width(bg, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_click(bg, false);
  
  // Create gradient sky (top 120px)
  sky = lv_obj_create(bg, nullptr);
  lv_obj_set_size(sky, 240, 120);
  lv_obj_set_pos(sky, 0, 0);
  
  // Create gradient effect for sky
  lv_obj_set_style_local_bg_color(sky, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x0A0A2E)); // Dark blue at top
  lv_obj_set_style_local_bg_grad_color(sky, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x2A2A5E)); // Lighter blue at bottom
  lv_obj_set_style_local_bg_grad_dir(sky, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
  lv_obj_set_style_local_border_width(sky, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_click(sky, false);
  
  // Create moon (instead of sun for a more magical feel)
  lv_obj_t * moon = lv_obj_create(sky, nullptr);
  lv_obj_set_size(moon, 30, 30);
  lv_obj_set_pos(moon, 180, 30);
  lv_obj_set_style_local_bg_color(moon, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xF0F0F0)); // Light silver
  lv_obj_set_style_local_border_width(moon, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_style_local_radius(moon, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
  lv_obj_set_click(moon, false);
  
  // Create stars
  for (int i = 0; i < 20; i++) {
    lv_obj_t * star = lv_obj_create(sky, nullptr);
    lv_obj_set_size(star, 2, 2);
    lv_obj_set_pos(star, rand() % 240, rand() % 120);
    lv_obj_set_style_local_bg_color(star, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF)); // White
    lv_obj_set_style_local_border_width(star, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_click(star, false);
  }
  
  // Create Hogwarts castle (simplified using rectangles)
  castle = lv_obj_create(bg, nullptr);
  lv_obj_set_size(castle, 140, 120);
  lv_obj_set_pos(castle, 50, 100);
  lv_obj_set_style_local_bg_color(castle, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x333333)); // Dark gray
  lv_obj_set_style_local_border_width(castle, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_click(castle, false);
  
  // Create castle towers
  // Left tower
  lv_obj_t * leftTower = lv_obj_create(bg, nullptr);
  lv_obj_set_size(leftTower, 30, 60);
  lv_obj_set_pos(leftTower, 30, 160);
  lv_obj_set_style_local_bg_color(leftTower, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x333333)); // Dark gray
  lv_obj_set_style_local_border_width(leftTower, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_click(leftTower, false);
  
  // Right tower
  lv_obj_t * rightTower = lv_obj_create(bg, nullptr);
  lv_obj_set_size(rightTower, 30, 60);
  lv_obj_set_pos(rightTower, 180, 160);
  lv_obj_set_style_local_bg_color(rightTower, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x333333)); // Dark gray
  lv_obj_set_style_local_border_width(rightTower, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_click(rightTower, false);
  
  // Center tower
  lv_obj_t * centerTower = lv_obj_create(bg, nullptr);
  lv_obj_set_size(centerTower, 40, 80);
  lv_obj_set_pos(centerTower, 100, 140);
  lv_obj_set_style_local_bg_color(centerTower, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x333333)); // Dark gray
  lv_obj_set_style_local_border_width(centerTower, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_click(centerTower, false);
  
  // Create house banner
  banner = lv_obj_create(bg, nullptr);
  lv_obj_set_size(banner, 120, 20);
  lv_obj_set_pos(banner, 60, 110);
  lv_obj_set_style_local_bg_color(banner, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, getPrimaryColor());
  lv_obj_set_style_local_border_width(banner, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_click(banner, false);
  
  // Move the background to the back
  lv_obj_move_background(bg);
}

void WatchFaceWarts::updateColorsForHouseTheme() {
  // Update banner color
  lv_obj_set_style_local_bg_color(banner, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, getPrimaryColor());
  
  // Update button color
  lv_obj_set_style_local_bg_color(houseButton, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, getPrimaryColor());
  
  // Update house label
  lv_label_set_text_static(houseLabel, getHouseName());
}

void WatchFaceWarts::cycleHouseTheme() {
  // Cycle through house themes
  switch (currentHouseTheme) {
    case HouseTheme::Lion:
      currentHouseTheme = HouseTheme::Snake;
      break;
    case HouseTheme::Snake:
      currentHouseTheme = HouseTheme::Eagle;
      break;
    case HouseTheme::Eagle:
      currentHouseTheme = HouseTheme::Badger;
      break;
    case HouseTheme::Badger:
      currentHouseTheme = HouseTheme::Lion;
      break;
  }
  
  updateColorsForHouseTheme();
}

lv_color_t WatchFaceWarts::getPrimaryColor() {
  switch (currentHouseTheme) {
    case HouseTheme::Lion:   return lv_color_hex(0x740001); // Gryffindor red
    case HouseTheme::Snake:  return lv_color_hex(0x2a623d); // Slytherin green
    case HouseTheme::Eagle:  return lv_color_hex(0x222f5b); // Ravenclaw blue
    case HouseTheme::Badger: return lv_color_hex(0xf0c75e); // Hufflepuff yellow
    default: return lv_color_hex(0x740001);
  }
}

lv_color_t WatchFaceWarts::getSecondaryColor() {
  switch (currentHouseTheme) {
    case HouseTheme::Lion:   return lv_color_hex(0xae0001); // Darker Gryffindor red
    case HouseTheme::Snake:  return lv_color_hex(0x1a472a); // Darker Slytherin green
    case HouseTheme::Eagle:  return lv_color_hex(0x0e1a40); // Darker Ravenclaw blue
    case HouseTheme::Badger: return lv_color_hex(0xecb939); // Darker Hufflepuff yellow
    default: return lv_color_hex(0xae0001);
  }
}

lv_color_t WatchFaceWarts::getAccentColor1() {
  switch (currentHouseTheme) {
    case HouseTheme::Lion:   return lv_color_hex(0xeeba30); // Gryffindor gold
    case HouseTheme::Snake:  return lv_color_hex(0x5d5d5d); // Slytherin gray
    case HouseTheme::Eagle:  return lv_color_hex(0xbebebe); // Ravenclaw silver
    case HouseTheme::Badger: return lv_color_hex(0x726255); // Hufflepuff brown
    default: return lv_color_hex(0xeeba30);
  }
}

lv_color_t WatchFaceWarts::getAccentColor2() {
  switch (currentHouseTheme) {
    case HouseTheme::Lion:   return lv_color_hex(0xd3a625); // Darker Gryffindor gold
    case HouseTheme::Snake:  return lv_color_hex(0xaaaaaa); // Lighter Slytherin gray
    case HouseTheme::Eagle:  return lv_color_hex(0x946b2d); // Ravenclaw bronze
    case HouseTheme::Badger: return lv_color_hex(0x372e29); // Darker Hufflepuff brown
    default: return lv_color_hex(0xd3a625);
  }
}

const char* WatchFaceWarts::getHouseName() {
  switch (currentHouseTheme) {
    case HouseTheme::Lion:   return "Lion";
    case HouseTheme::Snake:  return "Snake";
    case HouseTheme::Eagle:  return "Eagle";
    case HouseTheme::Badger: return "Badger";
    default: return "Lion";
  }
}

void WatchFaceWarts::Refresh() {
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
    }
  }

  heartbeat = heartRateController.HeartRate();
  heartbeatRunning = heartRateController.State() != Controllers::HeartRateController::States::Stopped;
  if (heartbeat.IsUpdated() || heartbeatRunning.IsUpdated()) {
    if (heartbeatRunning.Get()) {
      lv_obj_set_style_local_text_color(heartbeatIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));
      lv_label_set_text_fmt(heartbeatValue, "%d", heartbeat.Get());
    } else {
      lv_obj_set_style_local_text_color(heartbeatIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x808080));
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