"""
Travel Inspiration - A destination selection program.

This program makes suggestions of destinations to which you may be interested
in visiting. It basis its selection of a destination using simple matching
logic based on your preferences and interests.
"""

__author__ = "Benjamin Martin"
__copyright__ = "The University of Queensland, 2019"


from destinations import Destinations


# Prompts for the application.

CONTINENT_QUESTION = ("Which continents would you like to travel to?",
                      ["Asia", "Africa", "North America", "South America", "Europe", "Oceania", "Antarctica"])

COST_QUESTION = ("What is money to you?",
                 [('$$$', 'No object'),
                  ('$$', 'Spendable, so long as I get value from doing so'),
                  ('$', 'Extremely important; I want to spend as little as possible')])

CRIME_QUESTION = ("How much crime is acceptable when you travel?",
                  ['Low', 'Average', 'High'])

KIDS_QUESTION = ("Will you be travelling with children?",
                 ["Yes", "No"])

SEASON_QUESTION = ("Which seasons do you plan to travel in?",
                   ['Spring', 'Summer', 'Autumn', 'Winter'])

CLIMATE_QUESTION = ("What climate do you prefer?",
                    ["Cold", "Cool", "Moderate", "Warm", "Hot"])

QUESTIONS = [
    ('continent', CONTINENT_QUESTION, 'multiple_numeric'),
    ('cost', COST_QUESTION, 'string'),
    ('crime', CRIME_QUESTION, 'numeric'),
    ('kids', KIDS_QUESTION, 'numeric'),
    ('season', SEASON_QUESTION, 'multiple_numeric'),
    ('climate', CLIMATE_QUESTION, 'numeric')
]

INTEREST_QUESTIONS = [
    ['sports', 'sports'],
    ['wildlife', 'wildlife'],
    ['nature', 'nature'],
    ['historical', 'historical sites'],
    ['cuisine', 'fine dining'],
    ['adventure', 'adventure activities'],
    ['beach', 'the beach'],
]


def ask_question(question, options):
    """Asks a question with a list of answer options.

    Parameters:
        question (str): Text of the question to be displayed.
        options (list[tuple(str,str)]): List of the answer options, where
                                        First tuple value is selection value and
                                        Second tuple value is option text.

    Return:
        (str) Value entered by user to select their option.
    """
    print(question)
    for key, option in options:
        print(f"  {key}) {option}")

    return input('> ')


def ask_question_numeric_options(question, options):
    """Asks a question with a list of answer options, handling invalid input.

    Parameters:
        question (str): Text of the question to be displayed.
        options (list[str]): List of the answer options

    Return:
        (str) Option selected by user.
    """
    while True:
        response = ask_question(question, enumerate(options, start=1))
        try:
            response = int(response)
            if 1 <= response <= len(options):
                 return options[response - 1]
        except (ValueError, IndexError):
            pass
        print(f"\nI'm sorry, but {response} is not a valid choice.",
              "Please try again.\n")


def ask_question_str_options(question, options):
    """Asks a question with a list of answer options, handling invalid input.

    Parameters:
        question (str): Text of the question to be displayed.
        options (list[tuple(str,str)]): List of the answer options, where
                                        First tuple value is selection value and
                                        Second tuple value is option text.

    Return:
        (str) Option selected by user.
    """
    valid_responses = {key for key, _ in options}
    while True:
        response = ask_question(question, options)
        if response in valid_responses:
            return response
        print(f"\nI'm sorry, but {response} is not a valid choice.",
              "Please try again.\n")


def ask_question_multiple_numeric_options(question, options):
    """Asks a question with a list of answer options, handling invalid input.
       User may enter multiple valid answers.

    Parameters:
        question (str): Text of the question to be displayed.
        options (list[str]): List of the answer options

    Return:
        (list[str]) Options selected by user.
    """
    while True:
        response = ask_question(question, enumerate(options, start=1))
        responses = []
        try:
            for option in response.split(','):
                option = int(option)
                assert 1 <= option <= len(options)
                responses.append(options[option - 1])
        except (ValueError, IndexError, AssertionError):
            print(f"\nI'm sorry, but {response} is not a valid choice.",
                  "Please try again.\n")
        else:
            return responses


def ask_interest_question(label):
    """Asks the user about their level of interest in an activity.

    Parameters:
        label (str): Text describing the activity.

    Return:
        (int) Level of interest indicated by the user.
    """
    while True:
        print(f"How much do you like {label}? (-5 to 5)")
        response = input('> ')
        try:
            response = int(response)
            if -5 <= response <= 5:
                return response
        except ValueError:
            pass
        print(f"\nI'm sorry, but {response} is not a valid choice.",
              "Please try again.\n")


def main():
    """Determine a possible destination for a user based on their interests."""
    print("Welcome to Travel Inspiration!\n")
    name = input("What is your name? ")
    print(f"\nHi, {name}!\n")

    # Task 1: Questions & Inputs
    # Prompt the user for their travel preferences and interests.
    responses = {}
    interests = {}

    # Based on the type of question, decide how to get input from user.
    for key, (question, options), type_ in QUESTIONS:
        if type_ == "numeric":
            responses[key] = ask_question_numeric_options(question, options)
        elif type_ == "multiple_numeric":
            responses[key] = ask_question_multiple_numeric_options(question,
                                                                   options)
        elif type_ == "string":
            responses[key] = ask_question_str_options(question, options)
        else:
            raise ValueError(f"Unknown question type: {type_}")
        print()

    print("Now we would like to ask you some questions about your interests, on "
          "a scale of -5 to 5. -5 indicates strong dislike, whereas 5 indicates "
          "strong interest, and 0 indicates indifference.\n")

    for key, label in INTEREST_QUESTIONS:
        interests[key] = ask_interest_question(label)
        print()

    # Select the best matching destination based on the user's preferences.
    match = None    # Signal that no match has been found yet.

    # Tasks 2-6:
    for destination in Destinations().get_all():
        # If destination's continent, crime level, cost, kid friendliness, climate
        # does not match the user's preferences, skip to the next destination.
        if destination.get_continent().title() not in responses['continent']:
            continue

        if (CRIME_QUESTION[1].index(destination.get_crime().title())
            > CRIME_QUESTION[1].index(responses['crime'])):
            continue

        if len(destination.get_cost()) > len(responses['cost']):
            continue

        if responses['kids'] == "Yes" and not destination.is_kid_friendly():
            continue

        # Task 3: Climate & Season Factor
        if responses['climate'].lower() != destination.get_climate():
            continue

        if match is None:
            match = destination
        else:
            # Task 3-5 Logic if task 6 not attempted:
            # prev_score = match.get_season_factor(responses['season'][0].lower())
            # score = destination.get_season_factor(responses['season'][0].lower())
            # Task 6:
            # Based on the season factor, which season has the highest score.
            prev_score = max(match.get_season_factor(season.lower())
                                 for season in responses['season'])
            score = max(destination.get_season_factor(season.lower())
                            for season in responses['season'])

            # Task 4: Interests
            prev_interest_score = sum(response * match.get_interest_score(key)
                                          for key, response in interests.items())
            interest_score = sum(response * destination.get_interest_score(key)
                                     for key, response in interests.items())

            prev_score *= prev_interest_score
            score *= interest_score

            if score > prev_score:
                match = destination

    print("Thank you for answering all our questions.",
          "Your next travel destination is:")
    if match is None:
        print(match)
    else:
        print(match.get_name())


if __name__ == "__main__":
    main()
