# Python style guide: https://peps.python.org/pep-0008/
# Matplotlib colours: https://matplotlib.org/stable/gallery/color/named_colors.html

import numpy as np
import matplotlib.pyplot as plt

ALLOCATED_COLOUR = "red"
FREE_COLOUR = "green"

def graph_memory_layout(memory_layout):
    # Format: F(ree)/A(llocated)[number of bytes]P(adding)[number of bytes] [space] [next entry]

    # Extract infomation
    layout = memory_layout.split()
    labels = []
    sizes = []
    colours = []

    for memory in layout:
        first_char = memory[0]
        if (first_char == "A"):
            # TODO need to show the padding stuff in a new colour and stuff
            # TODO We can use an edge colour to show the edges

            labels.append("Allocated")
            sizes.append(memory[1:])
            colours.append(ALLOCATED_COLOUR)
        elif (first_char == "F"):
            labels.append("Free")
            sizes.append(memory[1:])
            colours.append(FREE_COLOUR)

    # Create pie chart 
    plt.pie(sizes, colors=colours, autopct='%1.1f%%') # labels=labels (for this graph, it can look cluttered with labels)
    plt.axis("equal")
    plt.title("Memory Usage")

     # Draw the chart
    plt.show()

def graph_memory_usage(memory_usage):
     # Format: A(llocated)[number of bytes] F(ree)[number of bytes]

    # Extract infomation
    usage = memory_usage.split()
    allocated = int(usage[0][1:])
    free = int(usage[1][1:])

    # Create pie chart
    sizes = [allocated, free]
    labels = ["Allocated", "Free"]
    colours = [ALLOCATED_COLOUR, FREE_COLOUR]

    plt.pie(sizes, labels=labels, colors=colours, autopct='%1.1f%%')
    plt.axis("equal")
    plt.title("Memory Usage")

     # Draw the chart
    plt.show()

def graph_full_memory_usage(memory_usage):
    # Format: Tag[number of bytes] [space] [next entry]

    # Extract infomation
    usage = memory_usage.split()
    labels = []
    sizes = []

    for memory in usage:
        bytes_start = -1
        for i, char in enumerate(memory):
            if char.isdigit():
                bytes_start = i
                break
        labels.append(memory[:bytes_start])
        sizes.append(memory[bytes_start:])

    # Create pie chart
    colours = plt.cm.tab10(np.arange(len(labels))) # Create a different colours for each entry

    plt.pie(sizes, labels=labels, colors=colours, autopct='%1.1f%%')
    plt.axis("equal")
    plt.title("Full Memory Usage")

    # Draw the chart
    plt.show()

def menu():
    # TODO make the program read from a file instead of copy/pasting in? For a large program, reading a file might be better?
    # TODO: have some sort of menu?
    pass

def main():
    #graph_memory_usage("A100 F50")
    #graph_full_memory_usage("Rendering100 Physics50 Free25")
    graph_memory_layout("A10 F10 A20 F10 A40 A20")

main()

# TODO rename file so not "main.py"? or have different files for the diferent graphs?
# TODO make the figure (window) titles different?

# TODO ctrl + f TODO
