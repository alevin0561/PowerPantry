import openai

# Initialize the OpenAI API client
openai.api_key = 'your-api-key'

def get_recipe_suggestions(food_items):
    prompt = f"Suggest a recipe using the following ingredients: {', '.join(food_items)}"
    
    response = openai.Completion.create(
        engine="text-davinci-003",
        prompt=prompt,
        max_tokens=150,
        n=1,
        stop=None,
        temperature=0.7,
    )
    
    recipe = response.choices[0].text.strip()
    return recipe

def chat():
    print("Welcome to the Recipe Suggestion Bot!")
    while True:
        user_input = input("Enter a list of food items (comma-separated) or 'quit' to exit: ")
        if user_input.lower() == 'quit':
            print("Goodbye!")
            break
        food_items = [item.strip() for item in user_input.split(',')]
        recipe = get_recipe_suggestions(food_items)
        print(f"Here's a recipe suggestion for you:\n{recipe}\n")

if __name__ == "__main__":
    chat()
