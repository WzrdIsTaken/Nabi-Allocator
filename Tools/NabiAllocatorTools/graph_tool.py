# Python style guide: https://peps.python.org/pep-0008/ && https://softwareengineering.stackexchange.com/questions/308972/python-file-naming-convention
# Matplotlib colours: https://matplotlib.org/stable/gallery/color/named_colors.html

import numpy as np
import matplotlib.pyplot as plt

ALLOCATED_COLOUR = "red"
FREE_COLOUR = "green"

def draw_pie_chart():
    # TODO implement, we have a lot of repeated code making the chart
    pass

def graph_memory_layout(memory_layout):
    # Format: F(ree)/A(llocated)[number of bytes]P(adding)[number of bytes] [space] [next entry]

    # Extract infomation
    layout = memory_layout.split()
    labels = []
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

            if padded_flag is not None:
                allocated_bytes = int(memory[1:padded_flag])
                padded_bytes = int(memory[padded_flag + 1:])
            else:
                allocated_bytes = memory[1:]

            # TODO need to show the padding stuff in a new colour and stuff
            # allocated_byes and padded_bytes equal the correct amounts

            labels.append("Allocated")
            sizes.append(allocated_bytes + padded_bytes)
            colours.append(ALLOCATED_COLOUR)
        elif (first_char == 'F'):
            labels.append("Free")
            sizes.append(memory[1:])
            colours.append(FREE_COLOUR)

    # Create pie chart 
    wedges = plt.pie(sizes, colors=colours) # labels=labels, autopct='%1.1f%%' (for this graph, it can look cluttered with labels / %s)
    for wedge in wedges[0]:
        wedge.set_lw(0.5)
        wedge.set_edgecolor("black")

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

    wedges = plt.pie(sizes, labels=labels, colors=colours, autopct='%1.1f%%')
    for wedge in wedges[0]:
        wedge.set_lw(0.5)
        wedge.set_edgecolor("black")

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
        wedge.set_lw(0.5)
        wedge.set_edgecolor("black")

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
    graph_memory_layout("A10P10 A10P0 F10")

main()

# TODO do we want main in this file..? gotta remind ourselves how python projects are set up
# TODO make the figure (window) titles different?
# TODO add some more functions and stuff (eg for making the pie chart) we have a lot of repeated code

# TODO ctrl + f TODO 
