#pragma once

#include "esphome/core/component.h"
#include "esphome/components/switch/switch.h"


namespace esphome {
	namespace enasa_hub {

		class EnasaHub;

		class TriggerOutput : public switch_::Switch {
		public:
			TriggerOutput(std::string name, std::function<void(void)> callback) : switch_::Switch(name) {
				callback_ = callback;
			}
			void write_state(bool state) override {
				callback_();
			}
		protected:
			std::function<void(void)> callback_;
			//void (*callback_)(void);
		};


	} //namespace empty_sensor_hub
} //namespace esphome