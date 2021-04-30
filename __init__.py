import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.components import binary_sensor
from esphome.components import text_sensor
from esphome.components import switch
from esphome.const import ( CONF_ID, CONF_NAME, ICON_EMPTY, UNIT_EMPTY )

AUTO_LOAD = ['sensor','text_sensor', 'binary_sensor', 'switch']
MULTI_CONF = True

CONF_HUB_ID = 'enasa_hub_id'

enasa_hub_ns = cg.esphome_ns.namespace('enasa_hub')

EnasaHub = enasa_hub_ns.class_('EnasaHub', cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(EnasaHub),
    cv.Required(CONF_NAME): cv.string,
    #cv.Optional("is_connected"): binary_sensor.BINARY_SENSOR_SCHEMA,
    #cv.Optional("is_playing"): binary_sensor.BINARY_SENSOR_SCHEMA,
    #cv.Optional("is_tracka_enabled"): binary_sensor.BINARY_SENSOR_SCHEMA,
    #cv.Optional("is_speaker_connected"): binary_sensor.BINARY_SENSOR_SCHEMA,
    #cv.Optional("is_headphones_connected"): binary_sensor.BINARY_SENSOR_SCHEMA,
    ##cv.Optional("genre"): text_sensor.TEXT_SENSOR_SCHEMA.extend( { cv.GenerateID(): cv.declare_id(text_sensor.TextSensor) } ),
    ##text_sensor.TEXT_SENSOR_SCHEMA,
    #cv.Optional("connect_headphones"): switch.SWITCH_SCHEMA,
}).extend(cv.COMPONENT_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    
    cg.add(var.set_name(config[CONF_NAME]))
    
    #if "is_connected" in config:
    #    sens = yield binary_sensor.new_binary_sensor(config["is_connected"])
    #    cg.add(var.set_is_connected_sensor(sens))
    #if "is_playing" in config:
    #    sens = yield binary_sensor.new_binary_sensor(config["is_playing"])
    #    cg.add(var.set_is_playing_sensor(sens))
    #if "is_tracka_enabled" in config:
    #    sens = yield binary_sensor.new_binary_sensor(config["is_tracka_enabled"])
    #    cg.add(var.set_is_tracka_enabled_sensor(sens))
    #if "is_speaker_connected" in config:
    #    sens = yield binary_sensor.new_binary_sensor(config["is_speaker_connected"])
    #    cg.add(var.set_is_speaker_connected_sensor(sens))
    #if "is_headphones_connected" in config:
    #    sens = yield binary_sensor.new_binary_sensor(config["is_headphones_connected"])
    #    cg.add(var.set_is_headphones_connected_sensor(sens))
        
    #if "genre" in config:
    #    configTemp = config["genre"]
    #    sens = cg.new_Pvariable(config["genre"][CONF_ID], "configTemp[CONF_NAME]")
    #    yield text_sensor.register_text_sensor(sens, config)
    #    cg.add(var.set_genre_sensor(sens))