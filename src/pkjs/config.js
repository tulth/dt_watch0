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
        "defaultValue": "0x000000",
        "label": "Time font color"
      },
      {
        "type": "color",
        "messageKey": "PrefDateFontColor",
        "defaultValue": "0x000000",
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
        "defaultValue": "0xFFFFFF",
        "label": "Time/Date background color"
      },
      {
        "type": "color",
        "messageKey": "PrefForecastBackgroundColor",
        "defaultValue": "0xFFFFFF",
        "label": "Forecast section background color"
      },
      {
        "type": "input",
        "messageKey": "PrefDateStrftimeStr",
        "defaultValue": "%a %d%b",
        "label": "Date strftime string"
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
