# to a python dev (or any dev..) this code is probs kinda spaghetti. but i'm not a python dev (and the code works :p)...
# Python style guide: https://peps.python.org/pep-0008/ && https://softwareengineering.stackexchange.com/questions/308972/python-file-naming-convention
# Matplotlib colours: https://matplotlib.org/stable/gallery/color/named_colors.html

import numpy as np
import matplotlib.patches as mPatch
import matplotlib.pyplot as plt

# Pie chart stuff
ALLOCATED_COLOUR = "orangered"
OUTLINE_COLOUR = "black"
OUTLINE_WIDTH = 1
PADDED_COLOUR = "salmon"
FREE_COLOUR = "greenyellow"

# Bar chart stuff
INDIVIDUAL_BAR_WIDTH = 0.27

def graph_memory_layout(memory_layout):
    # Format: F(ree)/A(llocated)[number of bytes]P(adding)[number of bytes] [space] [next entry] | Eg: A10P5 A10P5 F10P0 A20P0 A5P1 A15P3 A14P0 F15P0

    def find_padded_flag(memory):
        padded_flag = None
        for i, char in enumerate(memory[1:]):
            if char == 'P':
                padded_flag = i + 1
        return padded_flag

    # Extract infomation
    layout = memory_layout.split()
    sizes = []
    colours = []

    for memory in layout:
        first_char = memory[0]
        if (first_char == 'A'):
            allocated_bytes = 0
            padded_bytes = 0
            padded_flag = find_padded_flag(memory)

            allocated_bytes = int(memory[1:padded_flag])
            padded_bytes = int(memory[padded_flag + 1:])
            payload_bytes = allocated_bytes - padded_bytes

            sizes.append(payload_bytes)
            colours.append(ALLOCATED_COLOUR)

            if padded_bytes is not 0:
                sizes.append(padded_bytes)
                colours.append(PADDED_COLOUR)
        else:
            sizes.append(memory[1:find_padded_flag(memory)])
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
    circle = mPatch.Circle(center, radius, fill=False, edgecolor=OUTLINE_COLOUR, linewidth=OUTLINE_WIDTH)
    ax.add_patch(circle)

    legend_elements = [mPatch.Patch(color=ALLOCATED_COLOUR, label="Allocated"), mPatch.Patch(color=PADDED_COLOUR, label="Padded"), mPatch.Patch(color=FREE_COLOUR, label="Free")]
    ax.legend(handles=legend_elements, bbox_to_anchor=(1.05, 1), loc="upper right")

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

    fig, ax = plt.subplots()
    wedges = plt.pie(sizes, colors=colours, autopct='%1.1f%%') #labels=labels (We can use this param and remove the legend if we want to display the data like that)
    for wedge in wedges[0]:
        wedge.set_lw(OUTLINE_WIDTH)
        wedge.set_edgecolor(OUTLINE_COLOUR)

    plt.axis("equal")
    plt.title("Memory Usage")

    legend_elements = [mPatch.Patch(color=ALLOCATED_COLOUR, label="Allocated"), mPatch.Patch(color=FREE_COLOUR, label="Free")]
    ax.legend(handles=legend_elements, bbox_to_anchor=(1.05, 1), loc="upper right")

    # Draw the chart
    plt.show()

def graph_full_memory_usage(memory_usage):
    # Format: [tag][number of bytes] [space] [next entry] | Eg: Rendering100 Physics50 Free25

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

    fig, ax = plt.subplots()
    wedges = plt.pie(sizes, colors=colours, autopct='%1.1f%%') #labels=labels (We can use this param and remove the legend if we want to display the data like that)
    for wedge in wedges[0]:
        wedge.set_lw(OUTLINE_WIDTH)
        wedge.set_edgecolor(OUTLINE_COLOUR)

    legend_elements = []
    for i in range(0, len(labels)):
        legend_elements.append(mPatch.Patch(color=colours[i], label=labels[i]))
    ax.legend(handles=legend_elements, bbox_to_anchor=(1.125, 1), loc="upper right")

    plt.axis("equal")
    plt.title("Full Memory Usage")

    # Draw the chart
    plt.show()

def graph_benchmark_results(benchmark_results):
    # Format: [category] [space] [allocator[,allocator(s)(optional)] [space] [time[,time(s)(optional)]] [next entry] | Eg: 10kAllocThenFree FreeListAllocator,StackAllocator 0.01,0.005
    '''
        in case we need this... (though i don't think it looks good on this graph)

        def label_top_of_bar(bars):
            for bar in bars:
                height = bar.get_height()
                ax.text(bar.get_x() + bar.get_width()/2.0, 1.05 * height, '%d'%int(height), ha='center', va='bottom')
    '''

    # Extract infomation
    results = benchmark_results.split()
    categories = []
    allocators = []
    times = []

    i = 0
    while i < len(results):
        categories.append(results[i])
        allocators.append(results[i + 1].split(','))
        times.append([float(time) for time in results[i + 2].split(',')])
        i += 3

    unique_allocators = len(allocators[0])
    number_of_bar_groups = len(allocators)
    bar_group_locations = np.arange(number_of_bar_groups)
    bar_colours = plt.cm.tab10(np.arange(unique_allocators))

    # Create bar chart
    fig = plt.figure()
    ax = fig.add_subplot()

    bars = []
    for i in range(0, len(allocators)):
        for j in range(0, unique_allocators):
            allocator_times = [times[i][j] for i in range(len(times))]
            bars.append(ax.bar(bar_group_locations + (INDIVIDUAL_BAR_WIDTH * j), allocator_times, INDIVIDUAL_BAR_WIDTH, color=bar_colours[j]))

    ax.set_ylabel("Time (ms)")
    ax.set_xticks(bar_group_locations + INDIVIDUAL_BAR_WIDTH * (len(allocators) - (len(allocators) - 1)) / 2)
    ax.set_xticklabels(categories) 
    ax.legend(bars, allocators[0]) # See "bbox_to_anchor" and "loc" params used in the other graph's legends if we need to move this one

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
    elif option is "3":
        graph_full_memory_usage(data)
    else: # option is "4"
        graph_benchmark_results(data)

def menu():
    running = True
    while running:
        print("--- Nabi Graph Tool ---")
        print("Enter a menu option and a file path.\n")
    
        print("1) Memory Layout")
        print("2) Memory Usage")
        print("3) Full Memory Usage")
        print("4) Benchmark Results")
        print("5) Exit")

        # Example input: C:\Users\Ben\Desktop\Game Dev\NabiAllocator\Nabi-Allocator\logs\Test.txt
        user_input = input("")

        if (user_input[0] is '1' or user_input[0] is '2' or user_input[0] is '3' or user_input[0] is '4'):
            action_user_input(user_input[0], user_input[2:])
        elif (user_input == "5"):
            running = False
        else:
            print("Invalid input\n")

        print("")

if __name__ == '__main__': # We might have to restructure this main stuff if we add more tools
    menu()
