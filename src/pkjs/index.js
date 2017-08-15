"use strict";
var message_keys = require('message_keys');

var Clay = require('pebble-clay');
var clayConfig = require('./config');
var clay = new Clay(clayConfig, null, { autoHandleEvents: false });

var weather_hourly_test = require('./weather_example');

var weather_apikey = null;
var pebble_response = {
    icons: [39, 39, 39],
    times: ["tbd", "tbd", "tbd"],
    temps: ["tbd", "tbd", "tbd"]
}

// Listen for when the config is requested
function show_configuration_handler (e) {
    console.log('show configuration!');
    clay.config = clayConfig;
    Pebble.openURL(clay.generateUrl());
    
}
Pebble.addEventListener('showConfiguration', show_configuration_handler);

function webviewclosed_handler(e) {
    if (e && !e.response) {
        return;
    }
    // Get the keys and values from each config item
    var settings_dict = clay.getSettings(e.response, false);  // false means i'll manually back map keys
    if ('weather_apikey' in settings_dict) {
        weather_apikey = settings_dict.weather_apikey.value
        // console.log('weather_apikey: ', weather_apikey);
        if (weather_apikey !== null) {
            localStorage.setItem('weather_apikey', weather_apikey);
        } else {
            localStorage.removeItem('weather_apikey');
        }
        delete settings_dict.weather_apikey;
    }
    // Send settings values to watch side
    settings_dict = Clay.prepareSettingsForAppMessage(settings_dict);
    // console.log('settings_dict: ', JSON.stringify(settings_dict));
    Pebble.sendAppMessage(settings_dict, function(e) {
        console.log('Sent config data to Pebble');
    }, function(e) {
        console.log('Failed to send config data!');
        console.log(JSON.stringify(e));
    });
}
Pebble.addEventListener('webviewclosed', webviewclosed_handler);


function fetch_weather_json(latitude, longitude, process_response) {
    var url = "https://api.wunderground.com/api/" + weather_apikey + "/hourly/q/" + latitude + "," + longitude + ".json"
    console.log(url);
    //var url = "https://api.wunderground.com/api/" + weather_apikey + "/hourly/q/" + "AL/Madison.json"
    var debug = false;
    if (debug) {
        process_response(weather_hourly_test);
    } else {
        try {
            var req = new XMLHttpRequest();
            // console.log('url: ', url);
            req.open('GET', url, true);
            req.onload = function () {
                if (req.readyState === 4) {
                    if (req.status === 200) {
                        // console.log('req.responseText: ', req.responseText);
                        var response = JSON.parse(req.responseText);
                        // console.log('response: ', JSON.stringify(response));
                        if ("error" in response.response) {
                            console.log('Error in response');
                        } else {
                            process_response(response);
                        }
                    } else {
                        console.log('Error');
                    }
                }
            };
            req.send(null);
        } catch(err){
            // console.log("Unable to GET weather JSON: " + err.message);
        }
    }
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    // console.log('PebbleKit JS ready!');
      // Update s_js_ready on watch
      weather_apikey = localStorage.getItem('weather_apikey');
      Pebble.sendAppMessage({'JSReady': 1})                        
      // console.log(e.type);
  }
);

function get_icon_from_url(url) {
    return url.substr(url.lastIndexOf("/")+1).split('.')[0];
}

function icon_name_to_key(icon_name) {
    switch(icon_name) {
    case "chanceflurries":    return 1;
    case "chancerain":        return 2;
    case "chancesleet":       return 3;
    case "chancesnow":        return 4;
    case "chancetstorms":     return 5;
    case "clear":             return 6;
    case "cloudy":            return 7;
    case "flurries":          return 8;
    case "fog":               return 9;
    case "hazy":              return 10;
    case "mostlycloudy":      return 11;
    case "mostlysunny":       return 12;
    case "nt_chanceflurries": return 13;
    case "nt_chancerain":     return 14;
    case "nt_chancesleet":    return 15;
    case "nt_chancesnow":     return 16;
    case "nt_chancetstorms":  return 17;
    case "nt_clear":          return 18;
    case "nt_cloudy":         return 19;
    case "nt_flurries":       return 20;
    case "nt_fog":            return 21;
    case "nt_hazy":           return 22;
    case "nt_mostlycloudy":   return 23;
    case "nt_mostlysunny":    return 24;
    case "nt_partlycloudy":   return 25;
    case "nt_partlysunny":    return 26;
    case "nt_rain":           return 27;
    case "nt_sleet":          return 28;
    case "nt_snow":           return 29;
    case "nt_sunny":          return 30;
    case "nt_tstorms":        return 31;
    case "partlycloudy":      return 32;
    case "partlysunny":       return 33;
    case "rain":              return 34;
    case "sleet":             return 35;
    case "snow":              return 36;
    case "sunny":             return 37;
    case "tstorms":           return 38;
    default:                  return 39;
    }
}

function send_weather_to_pebble() {
    var icons_message = {}
    // console.log('send_weather_to_pebble')
    pebble_response.icons.forEach(function (icon, idx, na) {
        icons_message[message_keys.ResponseWeatherIcons + idx] = icon;
    });
    // console.log('icons_message: ', JSON.stringify(icons_message));
    Pebble.sendAppMessage(icons_message, function(e) {
        // console.log('Sent config data to Pebble');
    }, function(e) {
        // console.log('Failed to send weather response icons data!');
        // console.log(JSON.stringify(e));
    });
    //
    var times_message = {}
    pebble_response.times.forEach(function (time, idx, na) {
        times_message[message_keys.ResponseWeatherTimeStrs + idx] = time;
    });
    // console.log('times_message: ', JSON.stringify(times_message));
    Pebble.sendAppMessage(times_message, function(e) {
        // console.log('Sent config data to Pebble');
    }, function(e) {
        // console.log('Failed to send weather response times data!');
        // console.log(JSON.stringify(e));
    });
    //
    var temps_message = {}
    pebble_response.temps.forEach(function (temp, idx, na) {
        temps_message[message_keys.ResponseWeatherTempStrs + idx] = temp;
    });
    // console.log('temps_message: ', JSON.stringify(temps_message));
    Pebble.sendAppMessage(temps_message, function(e) {
        // console.log('Sent config data to Pebble');
    }, function(e) {
        // console.log('Failed to send weather response temps data!');
        // console.log(JSON.stringify(e));
    });   
}
function process_weather_and_send_to_pebble(weather_hourly) {
    var time_strs = []
    var icon_ids = []
    var temp_strs = []
    weather_hourly.hourly_forecast.forEach(function (hour_forecast, idx, arr) {
        var hour = parseInt(hour_forecast.FCTTIME.hour)
        if (idx == 0 || hour == 18 || hour == 8) {
            if (hour > 12) {
                hour -= 12;
            }
            if (idx == 0) {
                time_strs.push("now")
            } else {
                time_strs.push(hour.toString() + hour_forecast.FCTTIME.ampm.toLowerCase())
            }
            // console.log(JSON.stringify(hour_forecast.FCTTIME.pretty));
            var icon = get_icon_from_url(weather_hourly.hourly_forecast[idx].icon_url);
            var icon_id = icon_name_to_key(icon)
            icon_ids.push(icon_id)
            temp_strs.push(weather_hourly.hourly_forecast[idx].temp.english + '\u00B0')
        }
    });
    pebble_response.icons = icon_ids.slice(0, 3);
    // console.log(JSON.stringify(pebble_response.icons));
    pebble_response.times = time_strs.slice(0, 3);
    // console.log(JSON.stringify(pebble_response.times));
    pebble_response.temps = temp_strs.slice(0, 3);
    // console.log(JSON.stringify(pebble_response.temps));
    send_weather_to_pebble();
}

function fetch_weather() {
    // console.log("fetch_weather")
    var locationOptions = {
        // enableHighAccuracy: true,
        maximumAge: 60000,
        timeout: 15000
    };
    function locationSuccess(pos) {
        console.log('location success');
        var latitude = pos.coords.latitude;
        var longitude = pos.coords.longitude;
        fetch_weather_json(latitude, longitude, process_weather_and_send_to_pebble);
    }

    function locationError(err) {
        console.warn('location error (' + err.code + '): ' + err.message);
    }
    navigator.geolocation.getCurrentPosition(locationSuccess, locationError,
                                             locationOptions);
}

// Listen for when an AppMessage is received
function app_message_handler(e) {
    // console.log('AppMessage received!');
    // console.log(JSON.stringify(e));
    if ("payload" in e) {
        if ("FetchWeather" in e.payload) {
            fetch_weather();
        }
    }
}                     
Pebble.addEventListener('appmessage', app_message_handler);


