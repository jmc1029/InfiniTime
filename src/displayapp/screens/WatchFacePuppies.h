#pragma once
#include <lvgl/lvgl.h>
#include <chrono>
#include <cstdint>
#include <memory>
#include "displayapp/screens/Screen.h"
#include "displayapp/Controllers.h"
#include "components/ble/SimpleWeatherService.h"
#include "components/ble/BleController.h"
#include "utility/DirtyValue.h"
#include "displayapp/apps/Apps.h"
#include "displayapp/widgets/StatusIcons.h"

namespace Pinetime {
  namespace Controllers {
    class DateTime;
    class Settings;
    class Battery;
    class Ble;
    class AlarmController;
    class NotificationManager;
    class HeartRateController;
    class MotionController;
    class SimpleWeatherService;
    class FS;
  }

  namespace Applications {
    namespace Screens {

      class WatchFacePuppies : public Screen {
      public:
        WatchFacePuppies(Controllers::DateTime& dateTimeController,
                       const Controllers::Battery& batteryController,
                       const Controllers::Ble& bleController,
                       const Controllers::AlarmController& alarmController,
                       Controllers::NotificationManager& notificationManager,
                       Controllers::Settings& settingsController,
                       Controllers::HeartRateController& heartRateController,
                       Controllers::MotionController& motionController,
                       Controllers::SimpleWeatherService& weather);
        ~WatchFacePuppies() override;

        void Refresh() override;

        static bool IsAvailable(Pinetime::Controllers::FS& filesystem);

      private:
        uint8_t chargingBatteryPercent = 101; // not a mistake ;)
        TickType_t savedTick = 0;
        TickType_t chargingAnimationTick = 0;

        uint8_t displayedHour = -1;
        uint8_t displayedMinute = -1;

        Utility::DirtyValue<std::chrono::time_point<std::chrono::system_clock, std::chrono::minutes>> currentDateTime {};
        Utility::DirtyValue<uint32_t> stepCount {};
        Utility::DirtyValue<uint8_t> heartbeat {}; // Still needed for dirty check, even if not displayed
        Utility::DirtyValue<bool> heartbeatRunning {}; // Still needed for dirty check
        Utility::DirtyValue<bool> notificationState {};
        Utility::DirtyValue<bool> bleState {};
        Utility::DirtyValue<bool> bleRadioEnabled {};
        Utility::DirtyValue<uint8_t> batteryPercentRemaining {};
        Utility::DirtyValue<bool> isCharging {};
        Utility::DirtyValue<std::optional<Pinetime::Controllers::SimpleWeatherService::CurrentWeather>> currentWeather {};

        Utility::DirtyValue<std::chrono::time_point<std::chrono::system_clock, std::chrono::days>> currentDate;
        
        lv_obj_t* bleIcon;
        lv_obj_t* batteryIcon; // New battery icon
        lv_obj_t* label_time;
        lv_obj_t* label_time_ampm;
        lv_obj_t* label_date;
        // Removed: lv_obj_t* heartbeatIcon;
        // Removed: lv_obj_t* heartbeatValue;
        lv_obj_t* stepIcon;
        lv_obj_t* stepValue;
        lv_obj_t* notificationIcon;
        lv_obj_t* weatherIcon;
        lv_obj_t* temperature;
        lv_obj_t* sky;
        lv_obj_t* sun;
        lv_obj_t* hillsContainer;
        lv_obj_t* hills[6]; // Array for our hill circles
        lv_obj_t* wilsonImg; // Wilson the dog image

        Controllers::DateTime& dateTimeController;
        Controllers::NotificationManager& notificationManager;
        Controllers::Settings& settingsController;
        Controllers::HeartRateController& heartRateController;
        Controllers::MotionController& motionController;
        Controllers::SimpleWeatherService& weatherService;
        const Controllers::Battery& batteryController;
        const Controllers::Ble& bleController;

        lv_task_t* taskRefresh;
        Widgets::StatusIcons statusIcons;

        void createRollingHillsBackground();
        void updateSkyForWeatherAndTime();
        void updateWilsonImage();
      };
    }

    template <>
    struct WatchFaceTraits<WatchFace::Puppies> {
      static constexpr WatchFace watchFace = WatchFace::Puppies;
      static constexpr const char* name = "Puppies";

      static Screens::Screen* Create(AppControllers& controllers) {
        return new Screens::WatchFacePuppies(controllers.dateTimeController,
                                           controllers.batteryController,
                                           controllers.bleController,
                                           controllers.alarmController,
                                           controllers.notificationManager,
                                           controllers.settingsController,
                                           controllers.heartRateController,
                                           controllers.motionController,
                                           *controllers.weatherController
                                           );
      };

      static bool IsAvailable(Pinetime::Controllers::FS& filesystem) {
        return Screens::WatchFacePuppies::IsAvailable(filesystem);
      }
    };
  }
}