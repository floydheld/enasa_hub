#pragma once

#include "trigger_output.h"
#include "enasa_switch.h"

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"

#include "BluetoothSerial.h"
#include <queue>


namespace esphome {
	namespace enasa_hub {

		class EnasaHub : public Component {
		public:
			void setup() override;
			void loop() override;
			void dump_config() override;

			void set_name(std::string name) { this->name = name; }

		protected:
			std::string name;
			BluetoothSerial btSerial;
			std::queue<uint8_t> btQueue;

			binary_sensor::BinarySensor* is_connected;
			EnasaSwitch* playing;
			EnasaSwitch* speaker_connected;
			EnasaSwitch* headphones_connected;
			EnasaSwitch* tracka_enabled;
			text_sensor::TextSensor* genre;
			text_sensor::TextSensor* artist;
			text_sensor::TextSensor* trackTitle;
			text_sensor::TextSensor* yearAlbum;
			text_sensor::TextSensor* ldStr;
			text_sensor::TextSensor* pdStr;
			text_sensor::TextSensor* pcStr;
			text_sensor::TextSensor* durationStr;
			text_sensor::TextSensor* artistTrackTitleNext;
			TriggerOutput* playIfIdle;
			TriggerOutput* next;
			TriggerOutput* prev;

			void set_playing(bool state);
			void set_speaker_connected(bool state);
			void set_headphones_connected(bool state);
			void set_tracka_enabled(bool state);
			void triggerPlayIfIdle();
			void triggerNext();
			void triggerPrev();

			void btReadInput();
			void btHandleInput();
			void btPublish(std::string topic, std::string payload);
		};


	} //namespace empty_sensor_hub
} //namespace esphome