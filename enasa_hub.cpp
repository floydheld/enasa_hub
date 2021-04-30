#include "enasa_hub.h"
#include <esphome/core/application.h>
#include "esphome/core/log.h"

namespace esphome {
	namespace enasa_hub {

        using namespace std::placeholders;

		static const char* TAG = "enasa_hub.component";

		void EnasaHub::setup() {
            playing = new EnasaSwitch("enasa " + name + " playing", std::bind(&EnasaHub::set_playing, this, _1));
            App.register_switch(playing);
            speaker_connected = new EnasaSwitch("enasa " + name + " speaker connected", std::bind(&EnasaHub::set_speaker_connected, this, _1));
            App.register_switch(speaker_connected);
            headphones_connected = new EnasaSwitch("enasa " + name + " headphones connected", std::bind(&EnasaHub::set_headphones_connected, this, _1));
            App.register_switch(headphones_connected);
            tracka_enabled = new EnasaSwitch("enasa " + name + " tracka", std::bind(&EnasaHub::set_tracka_enabled, this, _1));
            App.register_switch(tracka_enabled);

            is_connected = new binary_sensor::BinarySensor("enasa " + name + " isConnected");
            App.register_binary_sensor(is_connected);
            genre = new text_sensor::TextSensor("enasa " + name + " genre");
            App.register_text_sensor(genre);
            artist = new text_sensor::TextSensor("enasa " + name + " artist");
            App.register_text_sensor(artist);
            trackTitle = new text_sensor::TextSensor("enasa " + name + " track title");
            App.register_text_sensor(trackTitle);
            yearAlbum = new text_sensor::TextSensor("enasa " + name + " year album");
            App.register_text_sensor(yearAlbum);
            ldStr = new text_sensor::TextSensor("enasa " + name + " load date");
            App.register_text_sensor(ldStr);
            pdStr = new text_sensor::TextSensor("enasa " + name + " play date");
            App.register_text_sensor(pdStr);
            pcStr = new text_sensor::TextSensor("enasa " + name + " play count esteem");
            App.register_text_sensor(pcStr);
            durationStr = new text_sensor::TextSensor("enasa " + name + " duration");
            App.register_text_sensor(durationStr);
            artistTrackTitleNext = new text_sensor::TextSensor("enasa " + name + " next");
            App.register_text_sensor(artistTrackTitleNext);

            playIfIdle = new TriggerOutput("enasa " + name + " play if idle", std::bind(&EnasaHub::triggerPlayIfIdle, this));
            App.register_switch(playIfIdle);
            next = new TriggerOutput("enasa " + name + " next", std::bind(&EnasaHub::triggerNext, this));
            App.register_switch(next);
            prev = new TriggerOutput("enasa " + name + " prev", std::bind(&EnasaHub::triggerPrev, this));
            App.register_switch(prev);

            char s[50];
            sprintf(s, "enasa hub %s", name.c_str());
            btSerial.begin(s);
		}


        void EnasaHub::loop() {
            btReadInput();
            btHandleInput();
        }


		void EnasaHub::dump_config() {
            ESP_LOGCONFIG(TAG, "enasa hub"); // +App.get_name());
            //LOG_BINARY_SENSOR(TAG, "  is connected", is_connected);
            //LOG_BINARY_SENSOR(TAG, "  is playing", is_playing);
            //LOG_BINARY_SENSOR(TAG, "  is tracka enabled", is_tracka_enabled);
            //LOG_BINARY_SENSOR(TAG, "  is speaker connected", is_speaker_connected);
            //LOG_BINARY_SENSOR(TAG, "  is headphones connected", is_headphones_connected);
            //LOG_TEXT_SENSOR(TAG, "  genre", genre);
		}



        void EnasaHub::set_playing(bool state) {
            ESP_LOGD(TAG, "set_playing triggered %s", state ? "on" : "off");
            btPublish("enasa/" + name + "/cmd/playback", state ? "play" : "pause");
        }


        void EnasaHub::set_speaker_connected(bool state) {
            ESP_LOGD(TAG, "set_speaker_connected triggered %s", state ? "on" : "off");
            btPublish("enasa/" + name + (state ? "/cmd/connectBt" : "/cmd/disconnectBt"), "speakers");
        }


        void EnasaHub::set_headphones_connected(bool state) {
            ESP_LOGD(TAG, "set_headphones_connected triggered %s", state ? "on" : "off");
            btPublish("enasa/" + name + (state ? "/cmd/connectBt" : "/cmd/disconnectBt"), "headphones");
        }


        void EnasaHub::set_tracka_enabled(bool state) {
            ESP_LOGD(TAG, "set_tracka_enabled triggered %s", state ? "on" : "off");
            btPublish("enasa/" + name + "/cmd/tracka", state ? "1" : "0");
        }


        void EnasaHub::triggerPlayIfIdle() {
            ESP_LOGD(TAG, "triggerPlayIfIdle triggered");
            btPublish("enasa/" + name + "/cmd/playback", "playIfIdle");
        }


        void EnasaHub::triggerNext() {
            ESP_LOGD(TAG, "triggerNext triggered");
            btPublish("enasa/" + name + "/cmd/playback", "next");
        }


        void EnasaHub::triggerPrev() {
            ESP_LOGD(TAG, "triggerPrev triggered");
            btPublish("enasa/" + name + "/cmd/playback", "prev");
        }



        void EnasaHub::btReadInput()
        {
            if (btSerial.connected(1))
            {
                int byteCount = 0;
                while (btSerial.available())
                {
                    btQueue.push(btSerial.read());
                    byteCount++;
                }
                if (byteCount > 0)
                {
                    ESP_LOGD(TAG, "%d bytes bt data", byteCount);
                }
                is_connected->publish_state(true);
            }
            else
            {
                is_connected->publish_state(false);
            }
        }


        void EnasaHub::btHandleInput()
        {
            if (!btQueue.size())
            {
                return;
            }
            int bytesToPreamble = 0;
            uint8_t pb0 = 0, pb1 = 0, pb2 = 0, pb3 = 0;
            while (pb0 != 0x08 || pb1 != 0x80 || pb2 != 0x4d || pb3 != 0x53)
            {
                if (!btQueue.size())
                {
                    ESP_LOGD(TAG, "skipped %d bt data but no preamble", bytesToPreamble);
                    return;
                }
                pb0 = pb1;
                pb1 = pb2;
                pb2 = pb3;
                pb3 = btQueue.front();
                btQueue.pop();
                bytesToPreamble++;
            }
            // preamble found!
            if (bytesToPreamble != 4)
            {
                ESP_LOGD(TAG, "bt data preamble found after %d bytes", bytesToPreamble);
            }
            if (btQueue.size() >= 12)
            {
                int topicLen = btQueue.front();
                btQueue.pop();
                topicLen |= btQueue.front() << 8;
                btQueue.pop();
                topicLen |= btQueue.front() << 16;
                btQueue.pop();
                topicLen |= btQueue.front() << 24;
                btQueue.pop();

                int payloadLen = btQueue.front();
                btQueue.pop();
                payloadLen |= btQueue.front() << 8;
                btQueue.pop();
                payloadLen |= btQueue.front() << 16;
                btQueue.pop();
                payloadLen |= btQueue.front() << 24;
                btQueue.pop();

                if (btQueue.size() >= topicLen)
                {
                    char topic[topicLen + 1] = { 0 }; // make it longer because a string needs termination
                    for (int i = 0; i < topicLen; i++)
                    {
                        topic[i] = (char)btQueue.front();
                        btQueue.pop();
                    }

                    if (btQueue.size() >= payloadLen)
                    {
                        char payload[payloadLen + 1] = { 0 }; // make it longer for termination for for the printf
                        for (int i = 0; i < payloadLen; i++)
                        {
                            payload[i] = btQueue.front();
                            btQueue.pop();
                        }

                        if (btQueue.size() >= 2)
                        {
                            //int qos = btQueue.front();
                            btQueue.pop();
                            bool retain = btQueue.front() > 0;
                            btQueue.pop();

                            ESP_LOGD(TAG, "Got %s  %s  (%d, %d, %d left)", topic, payload, topicLen, payloadLen, btQueue.size());
                            if (!("enasa/" + name + "/playing").compare(topic))
                            {
                                playing->publish_state(strcmp(payload, "0"));
                            }
                            else if (!("enasa/" + name + "/tracka").compare(topic))
                            {
                                tracka_enabled->publish_state(strcmp(payload, "0"));
                            }
                            else if (!("enasa/" + name + "/speakerConnected").compare(topic))
                            {
                                speaker_connected->publish_state(strcmp(payload, "0"));
                            }
                            else if (!("enasa/" + name + "/headphonesConnected").compare(topic))
                            {
                                headphones_connected->publish_state(strcmp(payload, "0"));
                            }
                            else if (!("enasa/" + name + "/genre").compare(topic))
                            {
                                genre->publish_state(payload);
                            }
                            else if (!("enasa/" + name + "/artist").compare(topic))
                            {
                                artist->publish_state(payload);
                            }
                            else if (!("enasa/" + name + "/trackTitle").compare(topic))
                            {
                                trackTitle->publish_state(payload);
                            }
                            else if (!("enasa/" + name + "/yearAlbum").compare(topic))
                            {
                                yearAlbum->publish_state(payload);
                            }
                            else if (!("enasa/" + name + "/ldStr").compare(topic))
                            {
                                ldStr->publish_state(payload);
                            }
                            else if (!("enasa/" + name + "/pdStr").compare(topic))
                            {
                                pdStr->publish_state(payload);
                            }
                            else if (!("enasa/" + name + "/pcStr").compare(topic))
                            {
                                pcStr->publish_state(payload);
                            }
                            else if (!("enasa/" + name + "/durationStr").compare(topic))
                            {
                                durationStr->publish_state(payload);
                            }
                            else if (!("enasa/" + name + "/artistTrackTitleNext").compare(topic))
                            {
                                artistTrackTitleNext->publish_state(payload);
                            }
                            else
                            {
                                ESP_LOGD(TAG, "...not a recognized topic");
                            }
                            // successfully parsed, no need for temporary queue no more
                            return;
                        }
                        else
                        {
                            ESP_LOGD(TAG, "bt data: topic len: %d, topic: %s, payload len: %d, payload: %s,  but queue length %d", topicLen, topic, payloadLen, payload, btQueue.size());
                        }
                    }
                    else
                    {
                        ESP_LOGD(TAG, "bt data: topic len: %d, topic: %s, payload len: %d,  but queue length %d", topicLen, topic, payloadLen, btQueue.size());
                    }
                }
                else
                {
                    ESP_LOGD(TAG, "bt data: topic len: %d, payload len: %d,  but queue length %d", topicLen, payloadLen, btQueue.size());
                }
            }
            else
            {
                ESP_LOGD(TAG, "bt data: queue length after preamble %d", btQueue.size());
            }
            // not successful
        }


        void EnasaHub::btPublish(std::string topic, std::string payload)
        {
            if (btSerial.connected(1))
            {
                btSerial.write(0x08);
                btSerial.write(0x80);
                btSerial.write(0x4d);
                btSerial.write(0x53);
                int topicLen = topic.length();
                btSerial.write((uint8_t*)&topicLen, 4);
                int payloadLen = payload.length();
                btSerial.write((uint8_t*)&payloadLen, 4);
                btSerial.write((uint8_t*)topic.c_str(), topicLen);
                btSerial.write((uint8_t*)payload.c_str(), payloadLen);
            }
        }

	} //namespace enasa_hub
} //namespace esphome

