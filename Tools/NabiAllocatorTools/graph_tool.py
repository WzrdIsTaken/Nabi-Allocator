# to a python dev (or any dev..) this code is probs kinda spaghetti. but i'm not a python dev (and the code works :p)...
# Python style guide: https://peps.python.org/pep-0008/ && https://softwareengineering.stackexchange.com/questions/308972/python-file-naming-convention
# Matplotlib colours: https://matplotlib.org/stable/gallery/color/named_colors.html

import numpy as np
import matplotlib.patches 
import matplotlib.pyplot as plt

ALLOCATED_COLOUR = "orangered"
OUTLINE_COLOUR = "black"
OUTLINE_WIDTH = 1
PADDED_COLOUR = "salmon"
FREE_COLOUR = "greenyellow"

def graph_memory_layout(memory_layout):
    # Format: F(ree)/A(llocated)[number of bytes]P(adding)[number of bytes] [space] [next entry] | Eg: A10P5 A10P5 F10 A20P0 A5P1 A15P3 A14P0 F15

    # Extract infomation
    layout = memory_layout.split()
    sizes = []
    colours = []

    for memory in layout:
        first_char = memory[0]
        if (first_char == 'A'):
            allocated_bytes = 0
            padded_bytes = 0
            padded_flag = None

            for i, char in enumerate(memory[1:]):
                if char == 'P':
                    padded_flag = i + 1

            allocated_bytes = int(memory[1:padded_flag])
            padded_bytes = int(memory[padded_flag + 1:])
            payload_bytes = allocated_bytes - padded_bytes

            sizes.append(payload_bytes)
            colours.append(ALLOCATED_COLOUR)

            if padded_bytes is not 0:
                sizes.append(padded_bytes)
                colours.append(PADDED_COLOUR)
        else:
            sizes.append(memory[1:])
            colours.append(FREE_COLOUR)

    # Create pie chart 
    fig, ax = plt.subplots()
    wedges, _ = plt.pie(sizes, colors=colours) # for this graph, it can look cluttered with labels / %s
    for i in range(0, len(wedges)):
        nonPaddedWedge = colours[i] is not PADDED_COLOUR
        wedges[i].set_linewidth(OUTLINE_WIDTH  if nonPaddedWedge else OUTLINE_WIDTH + 0.5)
        wedges[i].set_edgecolor(OUTLINE_COLOUR if nonPaddedWedge else PADDED_COLOUR)

    center = wedges[0].center
    radius = wedges[0].r
    circle = matplotlib.patches.Circle(center, radius, fill=False, edgecolor=OUTLINE_COLOUR, linewidth=OUTLINE_WIDTH)
    ax.add_patch(circle)

    plt.axis("equal")
    plt.title("Memory Layout")

     # Draw the chart
    plt.show()

def graph_memory_usage(memory_usage):
     # Format: A(llocated)[number of bytes] F(ree)[number of bytes] | Eg: A100 F50

    # Extract infomation
    usage = memory_usage.split()
    allocated = int(usage[0][1:])
    free = int(usage[1][1:])

    # Create pie chart
    sizes = [allocated, free]
    labels = ["Allocated", "Free"]
    colours = [ALLOCATED_COLOUR, FREE_COLOUR]

    wedges = plt.pie(sizes, labels=labels, colors=colours, autopct='%1.1f%%')
    for wedge in wedges[0]:
        wedge.set_lw(OUTLINE_WIDTH)
        wedge.set_edgecolor(OUTLINE_COLOUR)

    plt.axis("equal")
    plt.title("Memory Usage")

     # Draw the chart
    plt.show()

def graph_full_memory_usage(memory_usage):
    # Format: Tag[number of bytes] [space] [next entry] | Eg: Rendering100 Physics50 Free25

    # Extract infomation
    usage = memory_usage.split()
    labels = []
    sizes = []

    for memory in usage:
        bytes_start = None
        for i, char in enumerate(memory):
            if char.isdigit():
                bytes_start = i
                break
        labels.append(memory[:bytes_start])
        sizes.append(int(memory[bytes_start:]))

    # Create pie chart
    colours = plt.cm.tab10(np.arange(len(labels))) # Create a different colours for each entry

    wedges = plt.pie(sizes, labels=labels, colors=colours, autopct='%1.1f%%')
    for wedge in wedges[0]:
        wedge.set_lw(OUTLINE_WIDTH)
        wedge.set_edgecolor(OUTLINE_COLOUR)

    plt.axis("equal")
    plt.title("Full Memory Usage")

    # Draw the chart
    plt.show()

def parse_file(file_path):
    file = open(file_path, "r")
    output = file.read()
    file.close()
    return output 

def action_user_input(option, user_input):
    data = None
    if "RAW" in user_input:
        user_input = user_input[:-3]
        data = user_input
    else:
        data = parse_file(user_input)

    if option is "1":
        graph_memory_layout(data)
    elif option is "2":
        graph_memory_usage(data)
    else: # option is "3"
        graph_full_memory_usage(data)

def menu():
    running = True
    while running:
        print("--- Nabi Graph Tool ---")
        print("Enter a menu option and a file path.\n")
    
        print("1) Memory Layout")
        print("2) Memory Usage")
        print("3) Full Memory Usage")
        print("4) Exit")

        # Example input: C:\Users\Ben\Desktop\Game Dev\NabiAllocator\Nabi-Allocator\logs\Test.txt
        user_input = input("")

        if (user_input[0] is '1' or user_input[0] is '2' or user_input[0] is '3'):
            action_user_input(user_input[0], user_input[2:])
        elif (user_input == "4"):
            running = False
        else:
            print("Invalid input\n")

        print("")

if __name__ == '__main__': # We might have to restructure this main stuff if we add more tools
    menu()
