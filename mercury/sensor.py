import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart, sensor
from esphome.const import (CONF_ID, CONF_UPDATE_INTERVAL, ICON_CHEMICAL_WEAPON)

DEPENDENCIES = ['uart']