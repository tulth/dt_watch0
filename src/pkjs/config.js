module.exports = [
    {
        "type": "heading",
        "defaultValue": "App Configuration"
    },
    {
        "type": "section",
        "items": [
            {
                "type": "heading",
                "defaultValue": "Colors"
            },
            {
                "type": "color",
                "messageKey": "PrefTimeFontColor",
                "defaultValue": "0xffffff",
                "label": "Time font color"
            },
            {
                "type": "color",
                "messageKey": "PrefDateFontColor",
                "defaultValue": "0xAAAAAA",
                "label": "Date font color"
            },
            {
                "type": "color",
                "messageKey": "PrefWeatherFontColor",
                "defaultValue": "0x000000",
                "label": "Weather forecast area font color"
            },
            {
                "type": "color",
                "messageKey": "PrefTimeDateBackgroundColor",
                "defaultValue": "0x0000AA",
                "label": "Time/Date background color"
            },
            {
                "type": "color",
                "messageKey": "PrefForecastBackgroundColor",
                "defaultValue": "0x00AAFF",
                "label": "Forecast section background color"
            },
            {
                "type": "input",
                "messageKey": "PrefDateStrftimeStr",
                "defaultValue": "%a %d%b",
                "label": "Date strftime string"
            },
            {
                "type": "slider",
                "messageKey": "weather_forecast_time0",
                "defaultValue": 8,
                "min": 0,
                "max": 23,
                "step": 1,
                "label": "Forecast time 0 (use 24-hour time)"
            },
            {
                "type": "slider",
                "messageKey": "weather_forecast_time1",
                "defaultValue": 18,
                "min": 0,
                "max": 23,
                "step": 1,
                "label": "Forecast time 1 (use 24-hour time)"
            },
            {
                "type": "toggle",
                "messageKey": "weather_forecast_rel_not_abs",
                "defaultValue": false,
                "label": "Off=use absolute times for forecasts.  On=Use relative offsets for forecasts."
            },
            {
                "type": "input",
                "messageKey": "weather_apikey",
                "defaultValue": "",
                "label": "Weather underground API key"
            }
        ]
    },
    {
        "type": "submit",
        "defaultValue": "Save Settings"
    }
];
