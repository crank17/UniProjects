"""
    Simple application to help make decisions about the suitability of the
    weather for a planned event. Second assignment for CSSE1001/7030.

    Event: Represents details about an event that may be influenced by weather.
    EventDecider: Determines if predicted weather will impact on a planned event.
    UserInteraction: Simple textual interface to drive program.
"""

__author__ = "Ryan Ward"
__email__ = "ryan.ward@uqconnect.edu.au"

from weather_data import WeatherData
from prediction import WeatherPrediction, YesterdaysWeather, SimplePrediction, SophisticatedPrediction
# Import your SimplePrediction and SophisticatedPrediction classes once defined.

# Define your Event Class here


class Event(object):
    """Class containing the input from the user"""
    def __init__(self, name, outdoors, cover_available, time):
        """
        Parameters:
              name (str): Name of the event
              outdoors (bool): If the event is outdoors or not
              cover_available (bool): If the event has cover or not
              time (int): Time of the event
                """
        self._name = name
        self._outdoors = outdoors
        self._cover_available = cover_available
        self._time = time

    def get_name(self):
        """(str) returns the name of the event"""
        return str(self._name)

    def get_time(self):
        """(int) returns the time of the event"""
        return self._time

    def get_outdoors(self):
        """(bool) returns the boolean of the input if the event is outdoors or not"""
        return self._outdoors

    def get_cover_available(self):
        """(bool) returns the boolean of the input if there is cover available or not"""
        return self._cover_available

    def __str__(self):
        """(str) returns the string of all the event details"""
        return f"Event({self._name} @ {self._time}, {self.get_outdoors()}, {self.get_cover_available()})"


class EventDecision(object):
    """Uses event details to decide if predicted weather suits an event."""

    def __init__(self, event, prediction_model):
        """
        Parameters:
            event (Event): The event to determine its suitability.
            prediction_model (WeatherPrediction): Specific prediction model.
                           An object of a subclass of WeatherPrediction used
                           to predict the weather for the event.
        """
        self._event = event
        self._prediction_model = prediction_model

    def _temperature_factor(self):
        """
        Determines how advisable it is to continue with the event based on
        predicted temperature

        Return:
            (float) Temperature Factor
        """

        temp_low = self._prediction_model.low_temperature()
        temp_high = self._prediction_model.high_temperature()

        # Step 1 - adjust predicted temps based on humidity factor
        humidity_factor = 0
        humidity = self._prediction_model.humidity()
        if  humidity > 70:
            humidity_factor = humidity / 20

        if temp_high >= 0:
            adj_temp_high = temp_high + humidity_factor
        else:
            adj_temp_high = temp_high - humidity_factor

        if temp_low >= 0:
            adj_temp_low = temp_low + humidity_factor
        else:
            adj_temp_low = temp_low - humidity_factor

        #step 2 - calculate initial temperature factor
        if ((6 <= self._event.get_time() <= 19) and (self._event.get_outdoors() is True) and (adj_temp_high >= 30.0)):
            temperature_factor = ((adj_temp_high/-5) + 6)
        elif adj_temp_high >= 45:
            temperature_factor = (adj_temp_high/-5 + 6)
        elif ((0 <= self._event.get_time() <= 5) or (20 <= self._event.get_time() <= 23)) and (adj_temp_low < 5) and (adj_temp_high < 45):
            temperature_factor = (adj_temp_low / 5 - 1.1)
        elif (adj_temp_low > 15) and (adj_temp_high < 30):
            temperature_factor = ((adj_temp_high - adj_temp_low) / 5)
        else:
            temperature_factor = 0

        if temperature_factor < 0:
            if self._event.get_cover_available() is True:
                temperature_factor += 1
            elif 3 < self._prediction_model.wind_speed() < 10:
                temperature_factor += 1
            elif self._prediction_model.cloud_cover() > 4:
                temperature_factor += 1

        return temperature_factor

    def _rain_factor(self):
        """
        Determines how advisable it is to continue with the event based on
        predicted rainfall

        Return:
            (float) Rain Factor
        """
        chance_of_rain = self._prediction_model.chance_of_rain()
        if chance_of_rain < 20:
            rain_factor = ((chance_of_rain / -5) + 4)
        elif chance_of_rain > 50:
            rain_factor = (chance_of_rain / -20) + 1
        else:
            rain_factor = 0

        if ((self._event.get_cover_available() is True) and (self._event.get_outdoors() is True)) and self._prediction_model.wind_speed() < 5:
            rain_factor += 1
        elif rain_factor < 2 and self._prediction_model.wind_speed() > 15:
            rain_factor = ((self._prediction_model.wind_speed() / -15) + rain_factor)
        elif rain_factor < -9:
            rain_factor = -9

        return rain_factor

    def advisability(self):
        """Determine how advisable it is to continue with the planned event.

        Return:
            (float) Value in range of -5 to +5,
                    -5 is very bad, 0 is neutral, 5 is very beneficial
        """
        advisability_rank = self._temperature_factor() + self._rain_factor()

        if advisability_rank < -5:
            advisability_rank = -5
        if advisability_rank > 5:
            advisability_rank = 5

        return advisability_rank


class UserInteraction(object):
    """Simple textual interface to drive program."""

    def __init__(self):
        """
        Parameters:
              (None): Class is designed to create instance of another class, therefore does have parameters
        """
        self._event = None
        self._prediction_model = None
        self._event_name = None

    def get_event_details(self):
        """Prompt the user to enter details for an event.

        Return:
            (Event): An Event object containing the event details.
        """
        name = input("What is the name of the event? ")
        self._event_name = name
        while True:
            outdoors = input("Is the event outdoors? ")
            outdoors = outdoors.lower()
            if (outdoors == "y") or (outdoors == "yes"):
                outdoors = True
                break

            elif (outdoors == "n") or (outdoors == "no"):

                outdoors = False
                break
            else:
                print("Invalid input, please enter a valid option")
        while True:
            cover_available = input("Is there covered shelter? ")
            cover_available = cover_available.lower()
            if (cover_available == "y") or (cover_available == "yes"):
                cover_available = True
                break

            elif (cover_available == 'n') or (cover_available == "no"):
                cover_available = False
                break
            else:
                print("Invalid input, please enter a valid option")
        while True:
            time = (input("What time is the event? "))
            if time.isdigit() and 0 <= int(time) <= 23:
                time = int(time)
                break
            else:
                print("Invalid input, please enter a valid option")

        event = Event(name, outdoors, cover_available, time)
        #print(event)
        return event

    def get_prediction_model(self, weather_data):
        """Prompt the user to select the model for predicting the weather.

        Parameter:
            weather_data (WeatherData): Data used for predicting the weather.

        Return:
            (WeatherPrediction): Object of the selected prediction model.
        """
        number_days = 0
        while True:
            print("Select the weather prediction model you wish to use:")
            print("  1) Yesterday's weather.")
            print("  2) Simple Prediction")
            print("  3) Sophisticated Prediction")
            model_choice = input("> ")
            # Error handling can be added to this method.

            if model_choice.isdigit() and model_choice == "1" or model_choice == "2" or model_choice == "3":
                model_choice = model_choice
                break
            else:
                print("Invalid choice, please choose a valid input")



        while True:
            if model_choice == '1' :
                self._prediction_model = YesterdaysWeather(weather_data)
                break
            # Cater for other prediction models when they are implemented.
            if model_choice == "2":
                number_days = (input("Enter how many days of data you wish to use for making the prediction: "))
                if number_days.isdigit():
                    number_days = int(number_days)
                    self._prediction_model = SimplePrediction(weather_data, number_days)
                    break
                else:
                    print("Invalid choice, please enter a valid input")

            if model_choice == "3":
                number_days = (input("Enter how many days of data you wish to use for making the prediction: "))
                if number_days.isdigit():
                    number_days = int(number_days)
                    self._prediction_model = SophisticatedPrediction(weather_data, number_days)
                    break
                else:
                    print("Invalid choice, please enter a valid input")

        return self._prediction_model

    def output_advisability(self, impact):

        """Output how advisable it is to go ahead with the event.

        Parameter:
            impact (float): Impact of the weather on the event.
                            -5 is very bad, 0 is neutral, 5 is very beneficial
        """

        # The following print statement is an example of printing out the
        # class name of an object, which you may use for making the
        # advisability output more meaningful.
        print("based on", type(self._prediction_model).__name__, "model, the advisability of holding", self._event_name, "is"
              , impact)

    def another_check(self):
        """Ask user if they want to check using another prediction model.

        Return:
            (bool): True if user wants to check using another prediction model.
        """

        while True:
            check_again = input("Would you like to check again? ")

            check_again = check_again.lower()
            if check_again == "y" or check_again == "yes":

                check_again = True


            elif check_again == "n" or check_again == "no":
                check_again = False


            else:
                print("Invalid choice, please enter a valid input")

            
            return check_again


def main():
    """Main application's starting point."""
    check_again = True
    weather_data = WeatherData()
    weather_data.load("weather_data.csv")
    user_interface = UserInteraction()

    print("Let's determine how suitable your event is for the predicted weather.")
    event = user_interface.get_event_details()

    while check_again:
        prediction_model = user_interface.get_prediction_model(weather_data)
        decision = EventDecision(event, prediction_model)
        impact = decision.advisability()
        user_interface.output_advisability(impact)
        check_again = user_interface.another_check()


if __name__ == "__main__":
    main()
