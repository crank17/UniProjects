"""
    Prediction model classes used in the second assignment for CSSE1001/7030.

    WeatherPrediction: Defines the super class for all weather prediction models.
    YesterdaysWeather: Predict weather to be similar to yesterday's weather.
"""

__author__ = ""
__email__ = ""

from weather_data import WeatherData


class WeatherPrediction(object):
    """Superclass for all of the different weather prediction models."""

    def __init__(self, weather_data):
        """
        Parameters:
            weather_data (WeatherData): Collection of weather data.

        Pre-condition:
            weather_data.size() > 0
        """
        self._weather_data = weather_data

    def get_number_days(self):
        """(int) Number of days of data being used in prediction"""
        raise NotImplementedError

    def chance_of_rain(self):
        """(int) Percentage indicating chance of rain occurring."""
        raise NotImplementedError

    def high_temperature(self):
        """(float) Expected high temperature."""
        raise NotImplementedError

    def low_temperature(self):
        """(float) Expected low temperature."""
        raise NotImplementedError

    def humidity(self):
        """(int) Expected humidity."""
        raise NotImplementedError

    def cloud_cover(self):
        """(int) Expected amount of cloud cover."""
        raise NotImplementedError

    def wind_speed(self):
        """(int) Expected average wind speed."""
        raise NotImplementedError


class YesterdaysWeather(WeatherPrediction):
    """Simple prediction model, based on yesterday's weather."""

    def __init__(self, weather_data):
        """
        Parameters:
            weather_data (WeatherData): Collection of weather data.

        Pre-condition:
            weather_data.size() > 0
        """
        super().__init__(weather_data)
        self._yesterdays_weather = self._weather_data.get_data(1)
        self._yesterdays_weather = self._yesterdays_weather[0]

    def get_number_days(self):
        """(int) Number of days of data being used in prediction"""
        return 1

    def chance_of_rain(self):
        """(int) Percentage indicating chance of rain occurring."""
        # Amount of yesterday's rain indicating chance of it occurring.
        NO_RAIN = 0.1
        LITTLE_RAIN = 3
        SOME_RAIN = 8
        # Chance of rain occurring.
        NONE = 0
        MILD = 40
        PROBABLE = 75
        LIKELY = 90

        if self._yesterdays_weather.get_rainfall() < NO_RAIN:
            chance_of_rain = NONE
        elif self._yesterdays_weather.get_rainfall() < LITTLE_RAIN:
            chance_of_rain = MILD
        elif self._yesterdays_weather.get_rainfall() < SOME_RAIN:
            chance_of_rain = PROBABLE
        else:
            chance_of_rain = LIKELY

        return chance_of_rain

    def high_temperature(self):
        """(float) Expected high temperature."""
        return self._yesterdays_weather.get_high_temperature()

    def low_temperature(self):
        """(float) Expected low temperature."""
        return self._yesterdays_weather.get_low_temperature()

    def humidity(self):
        """(int) Expected humidity."""
        return self._yesterdays_weather.get_humidity()

    def wind_speed(self):
        """(int) Expected average wind speed."""
        return self._yesterdays_weather.get_average_wind_speed()

    def cloud_cover(self):
        """(int) Expected amount of cloud cover."""
        return self._yesterdays_weather.get_cloud_cover()


# Your implementations of the SimplePrediction and SophisticatedPrediction
# classes should go here.
class SimplePrediction(WeatherPrediction):
    """Simple Prediction model, based on the number of days the user inputs"""
    def __init__(self, weather_data, number_days):
        """

        Parameters:
        weather_data (WeatherData):  Collection of weather data
         number_days: The number of days inputted
        """
        super().__init__(weather_data)

        if(number_days > weather_data.size()):
            self._number_days = weather_data.size()
        else:
            self._number_days = number_days


    def get_number_days(self):
        """(int) number of days of data to be used in the prediction"""
        return self._number_days

    def chance_of_rain(self):
        """(int) returns the integer of the average chance of rain"""
        total_rainfall = 0
        weather_items = self._weather_data.get_data(self._number_days)
        for weather_item in weather_items:
            total_rainfall += weather_item.get_rainfall()

        av_rainfall = total_rainfall / self._number_days

        chance_of_rain = av_rainfall * 9
        if chance_of_rain > 100:
            chance_of_rain = 100

        return round(chance_of_rain)

    def high_temperature(self):
        """(int) returns the average high temperature"""
        high_temp = -273

        weather_items = self._weather_data.get_data(self._number_days)

        for weather_item in weather_items:
            if weather_item.get_high_temperature() > high_temp:
                high_temp = weather_item.get_high_temperature()

        return high_temp

    def low_temperature(self):
        """(int) returns the average low temperature"""
        low_temp = 273

        weather_items = self._weather_data.get_data(self._number_days)

        for weather_item in weather_items:
            if weather_item.get_low_temperature() < low_temp:
                low_temp = weather_item.get_low_temperature()

        return low_temp

    def humidity(self):
        """(int) returns the average humidity"""
        av_humidity = 0
        weather_items = self._weather_data.get_data(self._number_days)
        for weather_item in weather_items:
            av_humidity += weather_item.get_humidity()

        average_humidity = av_humidity / self._number_days

        return average_humidity

    def cloud_cover(self):
        """(int) returns the average cloud cover"""
        av_cloud_cover = 0
        weather_items = self._weather_data.get_data(self._number_days)
        for weather_item in weather_items:
            av_cloud_cover += weather_item.get_cloud_cover()

        cloud_cover = av_cloud_cover / self._number_days

        return round(cloud_cover)

    def wind_speed(self):
        """(int) returns the average wind speed"""
        av_wind_speed = 0
        weather_items = self._weather_data.get_data(self._number_days)
        for weather_item in weather_items:
            av_wind_speed += weather_item.get_average_wind_speed()

        wind_speed = av_wind_speed / self._number_days

        return round(wind_speed)


class SophisticatedPrediction(WeatherPrediction):
    """Sophisticated Prediction model, based on the number of days the user inputs"""
    def __init__(self, weather_data, number_days):
        """

        Parameters:
        weather_data (WeatherData):  Collection of weather data
         number_days: The number of days inputted
        """
        super().__init__(weather_data)

        if(number_days > weather_data.size()):
            self._number_days = weather_data.size()
        else:
            self._number_days = number_days

        # get yesterday's weather data item
        yesterdays_weather = self._weather_data.get_data(1)
        yesterdays_weather = yesterdays_weather[0]
        self._yesterdays_air_pressure = yesterdays_weather.get_air_pressure()
        self._yesterdays_wind_direction = yesterdays_weather.get_wind_direction()
        self._yesterdays_max_wind = yesterdays_weather.get_maximum_wind_speed()

    def get_average_pressure(self):
        """(int) returns the average air pressure over the number of days provided"""
        total_air_pressure = 0
        weather_items = self._weather_data.get_data(self._number_days)
        for weather_item in weather_items:
            total_air_pressure += weather_item.get_air_pressure()
        av_pressure = total_air_pressure / self._number_days
        return av_pressure


    def get_number_days(self):
        """(int) number of days of data to be used in the prediction"""
        return self._number_days

    def chance_of_rain(self):
        """(int) returns the integer of the average chance of rain"""
        total_rainfall = 0
        total_air_pressure = 0
        weather_items = self._weather_data.get_data(self._number_days)
        for weather_item in weather_items:
            total_rainfall += weather_item.get_rainfall()

        av_rainfall = total_rainfall / self._number_days

        if self._yesterdays_air_pressure < self.get_average_pressure():
            chance_of_rain = av_rainfall * 10
        else: #self._yesterdays_air_pressure >= av_pressure:
            chance_of_rain = av_rainfall * 7

        if "E" in self._yesterdays_wind_direction:
            chance_of_rain = chance_of_rain * 1.2

        if chance_of_rain > 100:
            chance_of_rain = 100

        return round(chance_of_rain)

    def high_temperature(self):
        """(int) returns the average high temperature"""
        high_temp = 0

        weather_items = self._weather_data.get_data(self._number_days)

        for weather_item in weather_items:
            if weather_item in weather_items:
                high_temp += weather_item.get_high_temperature()

        av_high = high_temp / self._number_days

        if self._yesterdays_air_pressure > self.get_average_pressure():
            av_high = av_high + 2
        return (av_high)

    def low_temperature(self):
        """(int) returns the average low temperature"""
        low_temp = 0

        weather_items = self._weather_data.get_data(self._number_days)

        for weather_item in weather_items:
            if weather_item in weather_items:
                low_temp += weather_item.get_low_temperature()
        av_low = low_temp / self._number_days

        if self._yesterdays_air_pressure < self.get_average_pressure():
            av_low = av_low - 2

        return (av_low)

    def humidity(self):
        """(int) returns the average humidity"""
        av_humidity = 0
        weather_items = self._weather_data.get_data(self._number_days)
        for weather_item in weather_items:
            av_humidity += weather_item.get_humidity()

        average_humidity = av_humidity / self._number_days

        if self._yesterdays_air_pressure < self.get_average_pressure():
            average_humidity += 15
        elif self._yesterdays_air_pressure > self.get_average_pressure():
            average_humidity = average_humidity - 15
        if average_humidity < 0:
            average_humidity = 0
        if average_humidity > 100:
            average_humidity = 100

        return round(average_humidity)

    def cloud_cover(self):
        """(int) returns the average cloud cover"""
        av_cloud_cover = 0
        weather_items = self._weather_data.get_data(self._number_days)
        for weather_item in weather_items:
            av_cloud_cover += weather_item.get_cloud_cover()

        cloud_cover = av_cloud_cover / self._number_days

        if self._yesterdays_air_pressure < self.get_average_pressure():
            cloud_cover += 2
        if cloud_cover > 9:
            cloud_cover = 9

        return round(cloud_cover)

    def wind_speed(self):
        """(int) returns the average wind speed"""
        av_wind_speed = 0
        weather_items = self._weather_data.get_data(self._number_days)
        for weather_item in weather_items:
            av_wind_speed += weather_item.get_average_wind_speed()

        wind_speed = av_wind_speed / self._number_days

        if self._yesterdays_max_wind > (4 * wind_speed):
            wind_speed = wind_speed * 1.2

        return round(wind_speed)


if __name__ == "__main__":
    print("This module provides the weather prediction models",
          "and is not meant to be executed on its own.")
