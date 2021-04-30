#pragma once

#include "esphome/core/component.h"
#include "esphome/components/switch/switch.h"


namespace esphome {
	namespace enasa_hub {

		class EnasaHub;

		class EnasaSwitch : public switch_::Switch {
		public:
			EnasaSwitch(std::string name, std::function<void(bool)> callback) : switch_::Switch(name) {
				callback_ = callback;
			}
			void write_state(bool state) override {
				callback_(state);
			}
		protected:
			std::function<void(bool)> callback_;
		};


	} //namespace empty_sensor_hub
} //namespace esphome