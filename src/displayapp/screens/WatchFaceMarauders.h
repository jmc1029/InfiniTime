#pragma once

#include <lvgl/src/lv_core/lv_obj.h>
#include <chrono>
#include <cstdint>
#include <memory>
#include "displayapp/screens/Screen.h"
#include "components/datetime/DateTimeController.h"
#include "components/ble/SimpleWeatherService.h"
#include "components/ble/BleController.h"
#include "displayapp/widgets/StatusIcons.h"
#include "utility/DirtyValue.h"
#include "displayapp/apps/Apps.h"

namespace Pinetime {
  namespace Controllers {
    class Settings;
    class Battery;
    class Ble;
    class NotificationManager;
    class HeartRateController;
    class MotionController;
  }

  namespace Applications {
    namespace Screens {

      class WatchFaceMarauders : public Screen {
      public:
        WatchFaceMarauders(Controllers::DateTime& dateTimeController,
                           const Controllers::Battery& batteryController,
                           const Controllers::Ble& bleController,
                           Controllers::NotificationManager& notificationManager,
                           Controllers::Settings& settingsController,
                           Controllers::HeartRateController& heartRateController,
                           Controllers::MotionController& motionController,
                           Controllers::SimpleWeatherService& weather,
                           Controllers::FS& fs);
        ~WatchFaceMarauders() override;

        void Refresh() override;

      private:
        uint8_t displayedHour = -1;
        uint8_t displayedMinute = -1;

        Utility::DirtyValue<std::chrono::time_point<std::chrono::system_clock, std::chrono::minutes>> currentDateTime {};
        Utility::DirtyValue<uint32_t> stepCount {};
        Utility::DirtyValue<uint8_t> heartbeat {};
        Utility::DirtyValue<bool> heartbeatRunning {};
        Utility::DirtyValue<std::optional<Pinetime::Controllers::SimpleWeatherService::CurrentWeather>> currentWeather {};

        lv_obj_t* background;
        
        // Splotch objects
        lv_obj_t* splotch_outer;
        lv_obj_t* splotch_mid;
        lv_obj_t* splotch_inner;
        lv_obj_t* splotch1;
        lv_obj_t* splotch2;
        lv_obj_t* splotch3;

        lv_obj_t* label_time;
        lv_obj_t* label_time_ampm;
        lv_obj_t* heartbeatIcon;
        lv_obj_t* heartbeatValue;
        lv_obj_t* stepIcon;
        lv_obj_t* stepValue;
        lv_obj_t* weatherIcon;
        lv_obj_t* temperature;

        Controllers::DateTime& dateTimeController;
        Controllers::NotificationManager& notificationManager;
        Controllers::Settings& settingsController;
        Controllers::HeartRateController& heartRateController;
        Controllers::MotionController& motionController;
        Controllers::SimpleWeatherService& weatherService;

        lv_font_t* font_harryP_Large = nullptr;
        lv_font_t* font_harryP_Small = nullptr;

        lv_task_t* taskRefresh;
        Widgets::StatusIcons statusIcons;
      };
    }

    template <>
    struct WatchFaceTraits<WatchFace::Marauders> {
      static constexpr WatchFace watchFace = WatchFace::Marauders;
      static constexpr const char* name = "Marauders face";

      static Screens::Screen* Create(AppControllers& controllers) {
        return new Screens::WatchFaceMarauders(controllers.dateTimeController,
                                               controllers.batteryController,
                                               controllers.bleController,
                                               controllers.notificationManager,
                                               controllers.settingsController,
                                               controllers.heartRateController,
                                               controllers.motionController,
                                               *controllers.weatherController,
                                               controllers.filesystem);
      };

      static bool IsAvailable(Pinetime::Controllers::FS& /*filesystem*/) {
        return true;
      }
    };
  }
}