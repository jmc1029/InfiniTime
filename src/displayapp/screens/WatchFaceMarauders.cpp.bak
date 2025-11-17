#include "displayapp/screens/WatchFaceMarauders.h"

#include <lvgl/lvgl.h>
#include <cstdio>

#include "displayapp/screens/NotificationIcon.h"
#include "displayapp/screens/Symbols.h"
#include "displayapp/screens/WeatherSymbols.h"
#include "displayapp/screens/BatteryIcon.h"
#include "components/battery/BatteryController.h"
#include "components/ble/BleController.h"
#include "components/ble/NotificationManager.h"
#include "components/heartrate/HeartRateController.h"
#include "components/motion/MotionController.h"
#include "components/ble/SimpleWeatherService.h"
#include "components/settings/Settings.h"

using namespace Pinetime::Applications::Screens;

WatchFaceMarauders::WatchFaceMarauders(Controllers::DateTime& dateTimeController,
                                       const Controllers::Battery& batteryController,
                                       const Controllers::Ble& bleController,
                                       Controllers::NotificationManager& notificationManager,
                                       Controllers::Settings& settingsController,
                                       Controllers::HeartRateController& heartRateController,
                                       Controllers::MotionController& motionController,
                                       Controllers::SimpleWeatherService& weatherService,
                                       Controllers::FS& filesystem)
  : currentDateTime {{}},
    dateTimeController {dateTimeController},
    notificationManager {notificationManager},
    settingsController {settingsController},
    heartRateController {heartRateController},
    motionController {motionController},
    weatherService {weatherService},
    statusIcons(batteryController, bleController) {

  // Create status icons first
  statusIcons.Create();

  // Load the Harry P font
  lfs_file f = {};
  if (filesystem.FileOpen(&f, "/fonts/HarryPotter_Large.bin", LFS_O_RDONLY) >= 0) {
    filesystem.FileClose(&f);
    font_harryP_Large = lv_font_load("F:/fonts/HarryPotter_Large.bin");
  }
  if (filesystem.FileOpen(&f, "/fonts/HarryPotter_Small.bin", LFS_O_RDONLY) >= 0) {
    filesystem.FileClose(&f);
    font_harryP_Small = lv_font_load("F:/fonts/HarryPotter_Small.bin");
  }

  // Create background with Marauder's Map color
  background = lv_obj_create(lv_scr_act(), nullptr);
  lv_obj_set_size(background, 240, 240);
  lv_obj_set_pos(background, 0, 0);
  lv_obj_set_style_local_bg_color(background, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xccb891));
  lv_obj_set_style_local_border_width(background, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_style_local_radius(background, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);

  // --- Create Central Splotch with Simulated Radial Gradient ---
  // Outermost, lightest circle
  splotch_outer = lv_obj_create(lv_scr_act(), nullptr);
  lv_obj_set_size(splotch_outer, 180, 180);
  lv_obj_align(splotch_outer, nullptr, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_style_local_bg_color(splotch_outer, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x8b0000));
  lv_obj_set_style_local_border_width(splotch_outer, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_style_local_radius(splotch_outer, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);

  // Middle circle
  splotch_mid = lv_obj_create(lv_scr_act(), nullptr);
  lv_obj_set_size(splotch_mid, 140, 140);
  lv_obj_align(splotch_mid, nullptr, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_style_local_bg_color(splotch_mid, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x6a0505));
  lv_obj_set_style_local_border_width(splotch_mid, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_style_local_radius(splotch_mid, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);

  // Innermost, darkest circle
  splotch_inner = lv_obj_create(lv_scr_act(), nullptr);
  lv_obj_set_size(splotch_inner, 100, 100);
  lv_obj_align(splotch_inner, nullptr, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_style_local_bg_color(splotch_inner, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x5a0606));
  lv_obj_set_style_local_border_width(splotch_inner, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_style_local_radius(splotch_inner, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);

  // --- Create Smaller Decorative Splotches ---
  splotch1 = lv_obj_create(lv_scr_act(), nullptr);
  lv_obj_set_size(splotch1, 40, 40);
  lv_obj_set_pos(splotch1, 30, 50);
  lv_obj_set_style_local_bg_color(splotch1, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x7a0606));
  lv_obj_set_style_local_border_width(splotch1, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_style_local_radius(splotch1, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);

  splotch2 = lv_obj_create(lv_scr_act(), nullptr);
  lv_obj_set_size(splotch2, 30, 30);
  lv_obj_set_pos(splotch2, 190, 60);
  lv_obj_set_style_local_bg_color(splotch2, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x5a0606));
  lv_obj_set_style_local_border_width(splotch2, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_style_local_radius(splotch2, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);

  splotch3 = lv_obj_create(lv_scr_act(), nullptr);
  lv_obj_set_size(splotch3, 50, 50);
  lv_obj_set_pos(splotch3, 170, 180);
  lv_obj_set_style_local_bg_color(splotch3, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x6a0505));
  lv_obj_set_style_local_border_width(splotch3, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_style_local_radius(splotch3, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);

  // Create time label in the center (on top of the splotch)
  label_time = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_font(label_time, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, font_harryP_Large);
  lv_obj_set_style_local_text_color(label_time, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xccb891));
  lv_obj_align(label_time, lv_scr_act(), LV_ALIGN_IN_LEFT_MID, 40, 10);

  label_time_ampm = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_text_static(label_time_ampm, "");
  lv_obj_set_style_local_text_color(label_time_ampm, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xccb891));
  lv_obj_set_style_local_text_font(label_time_ampm, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, font_harryP_Small);
  lv_obj_align(label_time_ampm, lv_scr_act(), LV_ALIGN_CENTER, -15, -35);

  // Create weather icon in top left
  weatherIcon = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(weatherIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x4d4735));
  lv_obj_set_style_local_text_font(weatherIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &fontawesome_weathericons);
  lv_label_set_text(weatherIcon, "");
  lv_obj_align(weatherIcon, nullptr, LV_ALIGN_IN_TOP_LEFT, 10, 10);

  temperature = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(temperature, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x4d4735));
  lv_label_set_text(temperature, "");
  lv_obj_align(temperature, weatherIcon, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

  // Create heartbeat icon and value in lower left
  heartbeatIcon = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_text_static(heartbeatIcon, Symbols::heartBeat);
  lv_obj_set_style_local_text_color(heartbeatIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x4d4735));
  lv_obj_align(heartbeatIcon, lv_scr_act(), LV_ALIGN_IN_BOTTOM_LEFT, 10, -10);

  heartbeatValue = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(heartbeatValue, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x4d4735));
  lv_label_set_text_static(heartbeatValue, "");
  lv_obj_align(heartbeatValue, heartbeatIcon, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

  // Create step icon and value in bottom right
  stepValue = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(stepValue, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x4d4735));
  lv_label_set_text_static(stepValue, "0");
  lv_obj_align(stepValue, lv_scr_act(), LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10);

  stepIcon = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(stepIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x4d4735));
  lv_label_set_text_static(stepIcon, Symbols::shoe);
  lv_obj_align(stepIcon, stepValue, LV_ALIGN_OUT_LEFT_MID, -5, 0);

  taskRefresh = lv_task_create(RefreshTaskCallback, LV_DISP_DEF_REFR_PERIOD, LV_TASK_PRIO_MID, this);
  Refresh();
}

WatchFaceMarauders::~WatchFaceMarauders() {
  lv_task_del(taskRefresh);
  lv_obj_clean(lv_scr_act());
}

void WatchFaceMarauders::Refresh() {
  statusIcons.Update();

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
  }

  heartbeat = heartRateController.HeartRate();
  heartbeatRunning = heartRateController.State() != Controllers::HeartRateController::States::Stopped;
  if (heartbeat.IsUpdated() || heartbeatRunning.IsUpdated()) {
    if (heartbeatRunning.Get()) {
      lv_label_set_text_fmt(heartbeatValue, "%d", heartbeat.Get());
    } else {
      lv_label_set_text_static(heartbeatValue, "");
    }
  }

  stepCount = motionController.NbSteps();
  if (stepCount.IsUpdated()) {
    lv_label_set_text_fmt(stepValue, "%lu", stepCount.Get());
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
  }
}