"""
Simple 2d world where the player can interact with the items in the world.
"""

__author__ = "Ryan Ward"
__date__ = "ryan.ward@uqconnect.edu.au"
__version__ = "1.1.0"
__copyright__ = "The University of Queensland, 2019"

import tkinter as tk
from tkinter import messagebox
import random
from collections import namedtuple
import pymunk
from block import Block, ResourceBlock, BREAK_TABLES, LeafBlock,\
    TrickCandleFlameBlock
from grid import Stack, Grid, SelectableGrid, ItemGridView
from item import Item, SimpleItem, HandItem, BlockItem, MATERIAL_TOOL_TYPES, \
    TOOL_DURABILITIES
from player import Player
from dropped_item import DroppedItem
from crafting import GridCrafter, CraftingWindow
from world import World
from core import positions_in_range
from game import GameView, WorldViewRouter, BoundaryWall
from mob import Bird
from sheep import Sheep
from furnace import FurnaceBlock, AdvancedCraftingWindow


BLOCK_SIZE = 2 ** 5
GRID_WIDTH = 2 ** 5
GRID_HEIGHT = 2 ** 4

# Task 3/Post-grad only:
# Class to hold game data that is passed to each thing's step function
# Normally, this class would be defined in a separate file
# so that type hinting could be used on PhysicalThing & its
# subclasses, but since it will likely need to be extended
# for these tasks, we have defined it here
GameData = namedtuple('GameData', ['world', 'player'])

class WorldViewRouter_new(WorldViewRouter):
    """Subclass of WorldViewRouter, used to change the routing table in order
    to draw additional mobs"""
    _routing_table = [
        # (class, method name)
        (Block, '_draw_block'),
        (TrickCandleFlameBlock, '_draw_mayhem_block'),
        (DroppedItem, '_draw_physical_item'),
        (Player, '_draw_player'),
        (Bird, '_draw_bird'),
        (Sheep, '_draw_sheep'),
        (BoundaryWall, '_draw_undefined'),
        (None.__class__, '_draw_undefined')
    ]

    def _draw_sheep(self, instance, shape, view):
        # draws a w white, rectangular sheep that routes to the Sheep class in
        # sheep.py
        return [view.create_rectangle(shape.bb.left, shape.bb.top,
                                      shape.bb.right, shape.bb.bottom,
                                 fill="white", tags=('mob', 'sheep'))]


def create_block(*block_id):
    """(Block) Creates a block (this function can be thought of as a block
     factory)

    Parameters:
        block_id (*tuple): N-length tuple to uniquely identify the block,
        often comprised of strings, but not necessarily (arguments are grouped
        into a single tuple)

    Examples:
        >>> create_block("leaf")
        LeafBlock()
        >>> create_block("stone")
        ResourceBlock('stone')
        >>> create_block("mayhem", 1)
        TrickCandleFlameBlock(1)
    """
    BREAK_TABLES_V2 = {
        "wool": {
            "hand": (1, True)
        },
        "wood_step": {
            "hand": (3, True)
        },
        "gold": {
            "hand": (70, False),
            "wood_pickaxe": (10, False),
            "stone_pickaxe": (7.5, False),
            "iron_pickaxe": (4, True),
            "diamond_pickaxe": (2, True),
            "golden_pickaxe": (1, True)
        },
        "iron": {
            "hand": (50, False),
            "wood_pickaxe": (10, False),
            "stone_pickaxe": (5, True),
            "iron_pickaxe": (3, True),
            "diamond_pickaxe": (2, True),
            "golden_pickaxe": (1, True)
        },
        "diamond": {
            "hand": (100, False),
            "wood_pickaxe": (10, False),
            "stone_pickaxe": (7.5, False),
            "iron_pickaxe": (5, True),
            "diamond_pickaxe": (2, True),
            "golden_pickaxe": (1, True)
        },
    }
    if len(block_id) == 1:
        block_id = block_id[0]
        if block_id == "leaf":
            return LeafBlock()
        elif block_id == 'crafting_table':
            return CraftingTableBlock(block_id, block_id[1])
        elif block_id == "furnace":
            return FurnaceBlock(block_id,block_id[1])
        elif block_id in BREAK_TABLES:
            return ResourceBlock(block_id, BREAK_TABLES[block_id])
        elif block_id in BREAK_TABLES_V2:
            return ResourceBlock(block_id, BREAK_TABLES_V2[block_id])

    elif block_id[0] == 'mayhem':
        return TrickCandleFlameBlock(block_id[1])
    elif block_id[0] == 'crafting_table':
        return CraftingTableBlock(block_id[0], block_id[1])
    raise KeyError(f"No block defined for {block_id}")


def create_item(*item_id):
    """(Item) Creates an item (this function can be thought of as a item
    factory)

    Parameters:
        item_id (*tuple): N-length tuple to uniquely identify the item,
        often comprised of strings, but not necessarily (arguments are grouped
        into a single tuple)

    Examples:
        >>> create_item("dirt")
        BlockItem('dirt')
        >>> create_item("hands")
        HandItem('hands')
        >>> create_item("pickaxe", "stone")  # *without* Task 2.1.2 implemented
        Traceback (most recent call last):
        ...
        NotImplementedError: "Tool creation is not yet handled"
        >>> create_item("pickaxe", "stone")  # *with* Task 2.1.2 implemented
        ToolItem('stone_pickaxe')

    """
    BlockItems = ['wood', 'stone', "crafting_table", 'furnace', 'wool',
                  'wood_step']
    NormalItems = ['stick', 'big_stick', 'wool_vest', 'cloth', 'blanket',
                   'charcoal', 'heated_blanket', 'iron', 'gold', 'diamond']

    if len(item_id) == 2:

        if item_id[0] in MATERIAL_TOOL_TYPES and item_id[1] in TOOL_DURABILITIES:
            return ToolItem(item_id[0], item_id[1], TOOL_DURABILITIES[item_id[1]])

    elif len(item_id) == 1:

        item_type = item_id[0]
        # Task 1.4 Basic Items: Create wood & stone here
        if item_type == "hands":
            return HandItem("hands")

        elif item_type == "dirt":
            return BlockItem(item_type)
        if item_type in BlockItems:
            return BlockItem(item_type)
        if item_type in NormalItems:
            return Item(item_type)

        elif item_type == "apple":
            return FoodItem(item_type, 2)

        elif item_type == 'boomerang':
            return ToolItem('', "boomer\nrang",200)

        elif item_type == 'cooked_apple':
            return FoodItem(item_type, 5)

        elif item_type == "burnt_apple":
            return FoodItem(item_type, -2)

        elif item_type == "fire_stick":
            return ToolItem("", item_type, 999999)

    raise KeyError(f"No item defined for {item_id}")


# Task 1.3: Implement StatusView class here
class StatusView(tk.Frame):
    """Class inherited from Frame in order to display the user's health
    and food status"""
    def __init__(self, master, game):
        """Constructor:
        Parameters:
            master: the tkinter root widg  et
            game: the instance of the Ninedraft class"""
        super().__init__(master)
        self.health_icon = tk.PhotoImage(file="health.png")
        self.hunger_icon = tk.PhotoImage(file = "hunger.png")
        self.health_image = tk.Label(self, image=self.health_icon).\
            grid(row=0, column=1)
        self.hunger_image = tk.Label(self, image=self.hunger_icon).\
            grid(row=0, column=3)

        self._health_label = tk.Label(self, text="Health: {}  ".
                                      format(game._player.get_health()),
                                   font=("fixedsys", 10))
        self._health_label.grid(row=0, column=2)

        self._food_label = tk.Label(self, text="Food: {}".
                                    format(game._player.get_food()),
                                   font=("fixedsys", 10))
        self._food_label.grid(row=0, column=4)

    def set_health(self, health):
        """updates health based on input"""
        self._health_label.config(text="Health: {}  ".format(health))

    def set_food(self, food):
        """Updates food based on input"""
        self._food_label.config(text="Food: {}".format(food))


class FoodItem(Item):
    """Food item class inherited from superclass Item"""
    def __init__(self, item_id: str, strength: float):
        """Constructor:
        Parameters:
            item_id: the string of the id of the item, e.g. the items name ("apple")
            strength: the strength of the item when used, e.g. if the strength
            of an apple is 2, will increase the health or food by 2 units"""
        super().__init__(item_id)
        self._strength = strength
        self._item_id = item_id

    def get_strength(self):
        """get the strength of the given item"""
        return self._strength

    def place(self):
        """Use the selected item"""
        return [("effect", ('food', self._strength))]
    def can_attack(self):
        """Items are not able to attack"""
        return False


class ToolItem(Item):
    """Tool Item class inherited from superclass Item"""
    def __init__(self, item_id, tool_type, durability):
        """Constructor:
        Parameters:
            item_id (str): the identification of the tool (name)
            tool_type (str): the material that the tool is made out of
            durability (float): the durability of the material"""
        super().__init__(item_id)
        self._id = f'{tool_type}_{item_id}'
        self._tool_type = tool_type
        self._durability = durability
        self._max_durability = durability

    def get_type(self):
        """gets the type of material and type of tool"""
        return self._tool_type

    def get_durability(self):
        """returns the durability of the tool"""
        return self._durability

    def get_max_durability(self):
        """returns the maximum durability of the tool"""
        return self._max_durability

    def can_attack(self):
        """Returns if the tool has run out of durability or not"""
        #if the tool's durability has run out, it will no longer be able
        # to mine or attack
        if self._durability > 0:
            return True
        else:
            return False

    def attack(self, successful):
        """returns if the attack is successful, reduce the durability by one"""
        if not successful:
            self._durability -= 1

    def is_stackable(self):
        """Tools are not stackable"""
        return False


class CraftingTableBlock(ResourceBlock):
    """Crafting table class to create the main source of crafting"""
    def __init__(self, block_id, break_table):
        """Constructor
        Parameters:
            block_id (str): The string identification of the block
            break_table (str): The specific break table of the block"""
        super().__init__(block_id, break_table)
        self._block_id = block_id
        self._block_id = 'crafting_table'
        self._hitpoints = 1

        #break table for crafting table, as it is wood, most logical to only
        # break with an axe and hands
        self._break_table = {
            "hand": (20, True),
            "wood_axe": (18, True),
            "stone_axe": (17, True),
            "iron_axe": (15, True),
            "diamond_axe": (10, True),
            "golden_axe": (10, True),
            }

    def get_block_id(self):
        return self._block_id

    def get_drops(self, luck, correct_item_used):
        """Drops a itself in item form 5 times

        See Block.get_drops for parameters & return"""

        if correct_item_used:
            return [('item', (self._block_id,))]

    def use(self):
        """On right click, open the crafting window"""
        return ('crafting', 'crafting_table')


    def __repr__(self):
        return f"ResourceBlock({self._id!r})"

BLOCK_COLOURS = {
    'diamond': 'blue',
    'dirt': '#552015',
    'stone': 'grey',
    'gold': "#FAFD22",
    'iron': "#BCC7D2",
    'wood': '#723f1c',
    'leaves': 'green',
    'wood_step':'tan2',
    'crafting_table': 'pink',
    'furnace': 'black',
    'wool':'white',
}

ITEM_COLOURS = {
    'diamond': 'blue',
    'dirt': '#552015',
    'gold': "#FAFD22",
    'iron': "#BCC7D2",
    'stone': 'grey',
    'wood': '#723f1c',
    'wood_step':'tan2',
    'apple': '#ff0000',
    'leaves': 'green',
    'crafting_table': 'pink',
    'furnace': 'black',
    'cooked_apple': 'red4',
    'wool': 'white'
}


def load_simple_world(world):
    """Loads blocks into a world

    Parameters:
        world (World): The game world to load with blocks
    """
    block_weights = [
        (100, 'dirt'),
        (30, 'stone'),
        (10, "iron"),
        (3, "gold"),
        (1, "diamond")
    ]

    cells = {}

    ground = []

    width, height = world.get_grid_size()

    for x in range(width):
        for y in range(height):
            if x < 22:
                if y <= 8:
                    continue
            else:
                if x + y < 30:
                    continue

            ground.append((x, y))

    weights, blocks = zip(*block_weights)
    kinds = random.choices(blocks, weights=weights, k=len(ground))

    for cell, block_id in zip(ground, kinds):
        cells[cell] = create_block(block_id)

    trunks = [(3, 8), (3, 7), (3, 6), (3, 5)]

    for trunk in trunks:
        cells[trunk] = create_block('wood')

    leaves = [(4, 3), (3, 3), (2, 3), (4, 2), (3, 2), (2, 2), (4, 4), (3, 4), (2, 4)]

    for leaf in leaves:
        cells[leaf] = create_block('leaf')

    for cell, block in cells.items():
        # cell -> box
        i, j = cell

        world.add_block_to_grid(block, i, j)

    world.add_block_to_grid(create_block("mayhem", 0), 14, 8)

    world.add_mob(Bird("friendly_bird", (12, 12)), 400, 100)
    world.add_mob(Sheep("sheep", (50, 40)), 400, 100)


CRAFTING_RECIPES_2x2 = [
    (
        (
            (None, 'wood'),
            (None, 'wood')
        ),
        Stack(create_item('stick'), 4)
    ),
    (
        (
            ('stick', 'stick'),
            (None, 'stick')
        ),
        Stack(create_item('boomerang'), 1)
    ),
    (
        (
            ('stick', None),
            (None, 'stick')
        ),
        Stack(create_item('big_stick'), 1)
    ),
    (
        (
            ('wood', 'wood'),
            ('wood', 'wood')
        ),
        Stack(create_item('crafting_table'), 1)
    ),
    (
        (
            ('wool', None),
            (None, None)
        ),
        Stack(create_item('cloth'), 4)
    ),
    (
        (
            ('wood', None),
            ('wood', 'wood')
        ),
        Stack(create_item('wood_step'), 1)
    )


]

#3x3 Crafting Recipes
CRAFTING_RECIPES_3x3 = {
    (
        (
            (None, None, None),
            (None, 'wood', None),
            (None, 'wood', None)
        ),
        Stack(create_item('stick'), 16)
    ),
    (
        (
            ('wood', 'wood', 'wood'),
            (None, 'stick', None),
            (None, 'stick', None)
        ),
        Stack(create_item('pickaxe', 'wood'), 1)
    ),
    (
        (
            ('stone', 'stone', 'stone'),
            (None, 'stick', None),
            (None, 'stick', None)
        ),
        Stack(create_item('pickaxe', 'stone'), 1)
    ),
    (
        (
            ('iron', 'iron', 'iron'),
            (None, 'stick', None),
            (None, 'stick', None)
        ),
        Stack(create_item('pickaxe', 'iron'), 1)
    ),
    (
        (
            ('diamond', 'diamond', 'diamond'),
            (None, 'stick', None),
            (None, 'stick', None)
        ),
        Stack(create_item('pickaxe', 'diamond'), 1)
    ),
    (
        (
            ('gold', 'gold', 'gold'),
            (None, 'stick', None),
            (None, 'stick', None)
        ),
        Stack(create_item('pickaxe', 'gold'), 1)
    ),
    (
        (
            ('wood', 'wood', None),
            ('wood', 'stick', None),
            (None, 'stick', None)
        ),
        Stack(create_item('axe', 'wood'), 1)
    ),
    (
        (
            (None, 'wood', None),
            (None, 'stick', None),
            (None, 'stick', None)
        ),
        Stack(create_item('shovel', 'wood'), 1)
    ),
    (
        (
            (None, 'stone', None),
            (None, 'stone', None),
            (None, 'stick', None)
        ),
        Stack(create_item('sword', 'wood'), 1)
    ),
    (

        (
            ('stone', 'stone', 'stone'),
            ('stone', None, 'stone'),
            ('stone', 'stone', 'stone')
        ),
        Stack(create_item('furnace'), 1)
    ),
    (

        (
            ('wool', None, 'wool'),
            ('wool', 'wool' , 'wool'),
            ('wool', 'wool', 'wool')
        ),
        Stack(create_item('wool_vest'), 1)
    ),
    (

        (
            ('cloth', "cloth", 'cloth'),
            ('cloth', 'cloth', 'cloth'),
            ('cloth', 'cloth', 'cloth')
        ),
        Stack(create_item('blanket'), 1)
    ),

}

CRAFTING_RECIPES_FURNACE = {
    (

        (

            ('apple',),
            (None,),
            ('wood',)

        ),
        Stack(create_item('cooked_apple'), 1)
    ),
    (

        (

            ('cooked_apple',),
            (None,),
            ('wood',)

        ),
        Stack(create_item('burnt_apple'), 1)
    ),
    (

        (

            ('wood',),
            (None,),
            ('wood',)

        ),
        Stack(create_item('charcoal'), 4)
    ),
    (

        (

            ('charcoal',),
            (None,),
            ('wood',)

        ),
        Stack(create_item('diamond'), 1)
    ),
    (

        (

            ('blanket',),
            (None,),
            ('wood',)

        ),
        Stack(create_item('heated_blanket'), 1)
    ),
    (

        (

            ('stick',),
            (None,),
            ('wood',)

        ),
        Stack(create_item('fire_stick'), 1)
    ),

}


class Ninedraft:
    """High-level app class for Ninedraft, a 2d sandbox game"""

    def __init__(self, master):
        """Constructor

        Parameters:
            master (tk.Tk): tkinter root widget
        """

        self._master = master
        #1.1 - Title
        master.title("Ninedraft")
        self._world = World((GRID_WIDTH, GRID_HEIGHT), BLOCK_SIZE)

        load_simple_world(self._world)

        self._player = Player()
        self._world.add_player(self._player, 250, 150)

        self._world.add_collision_handler("player", "item", on_begin=self.
                                          _handle_player_collide_item)

        self._hot_bar = SelectableGrid(rows=1, columns=10)
        self._hot_bar.select((0, 0))

        starting_hotbar = [
            Stack(create_item("dirt"), 20),
            Stack(create_item("apple"), 4),
            Stack(create_item('crafting_table'),1),
            Stack(create_item('furnace'), 1),
            Stack(create_item('pickaxe', 'diamond'), 1),
            Stack(create_item("axe", "diamond"),1)

        ]

        for i, item in enumerate(starting_hotbar):
            self._hot_bar[0, i] = item

        self._hands = create_item('hands')

        starting_inventory = [
            ((1, 5), Stack(create_item('dirt'), 10)),
            ((0, 2), Stack(create_item('wood'), 10)),
        ]
        self._inventory = Grid(rows=3, columns=10)
        for position, stack in starting_inventory:
            self._inventory[position] = stack

        self._crafting_window = None
        self._master.bind_all("e",
                          lambda e: self.run_effect(('crafting', 'basic')))

        self._view = GameView(master, self._world.get_pixel_size(),
                              WorldViewRouter_new(BLOCK_COLOURS, ITEM_COLOURS))
        self._view.pack()

        # Task 1.2 Mouse Controls: Bind mouse events here
        self._view.bind("<Motion>", self._mouse_move)
        self._view.bind("<Button-1>", self._left_click)
        #button-2 for functionailty with mac/other computers
        self._view.bind('<Button-2>', self._right_click)
        #button-3 is right click on Windows
        self._view.bind("<Button-3>", self._right_click)
        # tkinter <leave> handles when cursor leaves the widget
        self._view.bind("<Leave>", self._mouse_leave)


        # Task 1.3: Create instance of StatusView here
        self._status_view = StatusView(master, self)
        self._status_view.pack(side=tk.TOP)


        self._hot_bar_view = ItemGridView(master, self._hot_bar.get_size())
        self._hot_bar_view.pack(side=tk.TOP, fill=tk.X)

        # Task 1.5 Keyboard Controls: Bind to space bar for jumping here
        self._master.bind("<space>", lambda e: self._jump())

        self._master.bind("a", lambda e: self._move(-1, 0))
        self._master.bind("<Left>", lambda e: self._move(-1, 0))
        self._master.bind("d", lambda e: self._move(1, 0))
        self._master.bind("<Right>", lambda e: self._move(1, 0))
        self._master.bind("s", lambda e: self._move(0, 1))
        self._master.bind("<Down>", lambda e: self._move(0, 1))

        # Task 1.5 Keyboard Controls: Bind numbers to hotbar activation here
        for key in range(1, 10):
            self._master.bind(str(key), lambda e, key = key:
            self._activate_item(key-1))
        # 0 key gets confused with front of list, defined seperately.
        self._master.bind("0", lambda e: self._hot_bar.toggle_selection((0, 9)))

        # Task 1.6 File Menu & Dialogs: Add file menu here
        #Utilizing the tkinter menubar
        self.menubar = tk.Menu(master)
        master.config(menu=self.menubar)
        #tearoff removes dashed line at top of dropdown box
        file_menu = tk.Menu(self.menubar, tearoff=0)
        self.menubar.add_cascade(label="File", menu=file_menu)
        file_menu.add_command(label="Exit", command=self._popupmsg_exit)
        file_menu.add_command(label="New Game", command=self._popupmsg_restart)

        self._target_in_range = False

        self._target_position = 0, 0

        self.redraw()

        self.step()

    def _popupmsg_exit(self):
        """Display exit popup box on button click"""
        response = messagebox.askyesno("Exit?", "Do you want to exit?")
        if response:
            #Destroy widget
            self._master.destroy()

    def _popupmsg_restart(self):
        """Display restart popup box on button click"""
        response = messagebox.askyesno("Restart?", "Do you want to restart?")
        if response:
            #run restart
            self._restart()

    def _popupmsg_death(self):
        """Display restart popup box on player death"""
        response = messagebox.askyesno("You kicked the bucket!",
                                       "Do you want to respawn?")
        if response:
            #restart game
            self._restart()
        else:
            #close game
            self._master.destroy()

    def redraw(self):
        """Continuosly refresh and redraw all items in world in accordance with
        the step() function"""
        self._view.delete(tk.ALL)

        # physical things
        self._view.draw_physical(self._world.get_all_things())

        target_x, target_y = self._target_position
        target = self._world.get_block(target_x, target_y)
        cursor_position = self._world.grid_to_xy_centre(*self._world.xy_to_grid
        (target_x, target_y))

        # Task 1.2 Mouse Controls: Show/hide target here
        if self._target_in_range:
            self._view.show_target(self._player.get_position(), cursor_position)
        else:
            self._view.hide_target()

        # Task 1.3 StatusView: Update StatusView values here
        self._status_view.set_health(self._player.get_health())
        self._status_view.set_food(self._player.get_food())

        # hot bar
        self._hot_bar_view.render(self._hot_bar.items(), self._hot_bar.
                                  get_selected())

    def step(self):
        """Essentially the clock of the game, refreshes after every 15 ticks"""
        data = GameData(self._world, self._player)
        self._world.step(data)
        self.redraw()

        # Task 1.6 File Menu & Dialogs: Handle the player's death if necessary
        # ...
        if self._player.get_health() <= 0:
            self._popupmsg_death()

        self._master.after(15, self.step)

    def _move(self, dx, dy):
        """Changes player's velocity based on the movement input"""
        self.check_target()
        velocity = self._player.get_velocity()
        self._player.set_velocity((velocity.x + dx * 80, velocity.y + dy * 80))


    def _jump(self):
        """Makes the player move up if space bar is pressed"""
        self.check_target()
        velocity = self._player.get_velocity()
        # Task 1.2: Update the player's velocity here
        # X velocity is set to slow down the player, Y velocity jumps 200 units up
        self._player.set_velocity((velocity.x * 0.55, velocity.y + -200))


    def mine_block(self, block, x, y):
        """When a player left clicks on the block with the right tool, it will
        break, disappearing and dropping items of themselves"""
        luck = random.random()

        active_item, effective_item = self.get_holding()

        was_item_suitable, was_attack_successful = block.mine(effective_item,
                                                              active_item, luck)

        effective_item.attack(was_attack_successful)

        if block.is_mined():
            # Task 1.2 Mouse Controls: Reduce the player's food/health appropriately

            if self._player.get_food() > 0:
                self._player.change_food(-0.5)
                self._status_view.set_food(self._player.get_food())
            else:
                self._player.change_health(-0.5)
                self._status_view.set_health(self._player.get_health())

            # Task 1.2 Mouse Controls: Remove the block from the world &
            # get its drops
            drops = block.get_drops(luck, was_item_suitable)
            self._world.remove_block(block)

            if not drops:
                return

            x0, y0 = block.get_position()

            for i, (drop_category, drop_types) in enumerate(drops):
                print(f'Dropped {drop_category}, {drop_types}')

                if drop_category == "item":
                    physical = DroppedItem(create_item(*drop_types))

                    # this is so bleh
                    x = x0 - BLOCK_SIZE // 2 + 5 + (i % 3) * 11 + random.randint(0, 2)
                    y = y0 - BLOCK_SIZE // 2 + 5 + ((i // 3) % 3) * 11 + random.randint(0, 2)

                    self._world.add_item(physical, x, y)
                elif drop_category == "block":
                    self._world.add_block(create_block(*drop_types), x, y)
                else:
                    raise KeyError(f"Unknown drop category {drop_category}")

    def get_holding(self):
        """Returns the active item that the player is holding"""
        active_stack = self._hot_bar.get_selected_value()
        active_item = active_stack.get_item() if active_stack else self._hands

        effective_item = active_item if active_item.can_attack() else self._hands

        return active_item, effective_item

    def check_target(self):
        """Checks where the cursor is in the game window"""
        # select target block, if possible
        active_item, effective_item = self.get_holding()

        pixel_range = active_item.get_attack_range() * self._world.get_cell_expanse()

        self._target_in_range = positions_in_range(self._player.get_position(),
                                                   self._target_position,
                                                   pixel_range)

    def _mouse_move(self, event):
        """Tracks the mouse movement in the game window"""
        #trace the mouse movement
        self._target_position = event.x, event.y
        self.check_target()

    def _mouse_leave(self, event):
        """Checks if the mouse has left the widget, which then hides the target"""
        # if target is out of range, return false
        self._target_in_range = False

    def _restart(self):
        """When the player dies, or the restart button is pressed, run this
        function"""
        # Restart function, essentially resets all of the __init__ values of
        # Ninedraft

        self._world = World((GRID_WIDTH, GRID_HEIGHT), BLOCK_SIZE)

        load_simple_world(self._world)

        self._player = Player()
        self._world.add_player(self._player, 250, 150)
        self._status_view.set_health(self._player.get_health())
        self._status_view.set_food(self._player.get_food())

        self._hot_bar = SelectableGrid(rows=1, columns=10)
        self._hot_bar.select((0, 0))

        starting_hotbar = [
            Stack(create_item("dirt"), 20),
            Stack(create_item('apple'), 4)
        ]

        for i, item in enumerate(starting_hotbar):
            self._hot_bar[0, i] = item

        self._hands = create_item('hands')

        starting_inventory = [
            ((1, 5), Stack(Item('dirt'), 10)),
            ((0, 2), Stack(Item('wood'), 10)),
        ]
        self._inventory = Grid(rows=3, columns=10)
        for position, stack in starting_inventory:
            self._inventory[position] = stack
        self._world.add_collision_handler("player", "item", on_begin=self.
                                          _handle_player_collide_item)

    def _left_click(self, event):
        """Handles the events and effects when the left click button is pressed"""
        # Invariant: (event.x, event.y) == self._target_position
        #  => Due to mouse move setting target position to cursor
        x, y = self._target_position

        if self._target_in_range:
            block = self._world.get_block(x, y)

            if block:
                self.mine_block(block, x, y)
            else:
                mobs = self._world.get_mobs(x, y, max_distance=(2**5))
                for mob in mobs:
                    effect = mob.attack(0)
                    if not effect:
                        continue
                    x0, y0 = mob.get_position()

                    for i, (drop_category, drop_types) in enumerate(effect):
                        print(f'Dropped {drop_category}, {drop_types}')

                        if drop_category == "item":
                            physical = DroppedItem(create_item(*drop_types))
                            x = x0 - BLOCK_SIZE // 2 + 5 + (
                                       i % 3) * 11 + random.randint(0, 2)
                            y = y0 - BLOCK_SIZE // 2 + 5 + (
                                        (i // 3) % 3) * 11 + random.randint(
                                0, 2)

                            self._world.add_item(physical, x, y)

    def _trigger_crafting(self, craft_type):
        """Handles the event when a certain type of crafting is called by pressing
        'e' or right clicking on the furnace or the crafting table"""

        print(f"Crafting with {craft_type}")
        crafter = None
        title = None

        if craft_type == "basic":
            crafter = GridCrafter(CRAFTING_RECIPES_2x2)
            title = "Basic Crafting"
            self._crafting_window = CraftingWindow(self._master, title,
                                                   self._hot_bar,
                                                   self._inventory,
                                                   crafter)
        if craft_type == "crafting_table":
            crafter = GridCrafter(CRAFTING_RECIPES_3x3, rows=3, columns=3)
            title = "Crafting Table"
            self._crafting_window = CraftingWindow(self._master, title,
                                                   self._hot_bar,
                                                   self._inventory,
                                                   crafter)
        if craft_type == 'furnace':
            crafter = GridCrafter(CRAFTING_RECIPES_FURNACE, rows=3, columns=1)
            title = "Smelting"
            self._crafting_window = AdvancedCraftingWindow(self._master, title,
                                               self._hot_bar, self._inventory,
                                               crafter)

    def run_effect(self, effect):
        """Executes the effect given to the function
        i.e. run crafting, update health or food"""
        if len(effect) == 2:
            if effect[0] == "crafting":
                craft_type = effect[1]

                if craft_type == "basic":
                    print("Can't craft much on a 2x2 grid :/")

                elif craft_type == "crafting_table":
                    print("Let's get our kraft® on! King of the brands")
                elif craft_type == "furnace":
                    print("It's gettin hot in here...")
                # Allow crafting window to toggle off and on
                if self._crafting_window == None:
                    self._trigger_crafting(craft_type)
                else:
                    self._crafting_window.destroy()
                    self._crafting_window = None

                return
            elif effect[0] in ("food", "health"):
                stat, strength = effect
                if self._player.get_food() >= self._player.get_max_food():
                    print(f"Gaining {strength} {'health'}!")
                    getattr(self._player, f"change_{'health'}")(strength)
                else:
                    print(f"Gaining {strength} {stat}!")
                    getattr(self._player, f"change_{stat}")(strength)
                return

        raise KeyError(f"No effect defined for {effect}")

    def _right_click(self, event):
        """Handles the event when the right mouse button is pressed"""
        print("Right click")

        x, y = self._target_position
        target = self._world.get_thing(x, y)

        if target:
            # use this thing
            print(f'using {target}')
            effect = target.use()
            print(f'used {target} and got {effect}')

            if effect:
                self.run_effect(effect)

        else:
            # place active item
            selected = self._hot_bar.get_selected()

            if not selected:
                return

            stack = self._hot_bar[selected]
            drops = stack.get_item().place()

            stack.subtract(1)
            if stack.get_quantity() == 0:
                # remove from hotbar
                self._hot_bar[selected] = None

            if not drops:
                return

            # handling multiple drops would be somewhat finicky, so prevent it
            if len(drops) > 1:
                raise NotImplementedError("Cannot handle dropping more than 1 thing")

            drop_category, drop_types = drops[0]

            x, y = event.x, event.y

            if drop_category == "block":
                existing_block = self._world.get_block(x, y)

                if not existing_block:
                    self._world.add_block(create_block(drop_types[0]), x, y)
                else:
                    raise NotImplementedError(
                        "Automatically placing a block nearby if the target cell is full is not yet implemented")

            elif drop_category == "effect":
                self.run_effect(drop_types)

            else:
                raise KeyError(f"Unknown drop category {drop_category}")

    def _activate_item(self, index):
        """Selects the desired item in the hotbar"""
        print(f"Activating {index}")

        self._hot_bar.toggle_selection((0, index))

    def _handle_player_collide_item(self, player: Player, dropped_item: DroppedItem, data,
                                    arbiter: pymunk.Arbiter):
        """Callback to handle collision between the player and a (dropped) item. If the player has sufficient space in
        their to pick up the item, the item will be removed from the game world.

        Parameters:
            player (Player): The player that was involved in the collision
            dropped_item (DroppedItem): The (dropped) item that the player collided with
            data (dict): data that was added with this collision handler (see data parameter in
                         World.add_collision_handler)
            arbiter (pymunk.Arbiter): Data about a collision
                                      (see http://www.pymunk.org/en/latest/pymunk.html#pymunk.Arbiter)
                                      NOTE: you probably won't need this
        Return:
             bool: False (always ignore this type of collision)
                   (more generally, collision callbacks return True iff the collision should be considered valid; i.e.
                   returning False makes the world ignore the collision)
        """

        item = dropped_item.get_item()

        if self._hot_bar.add_item(item):
            print(f"Added 1 {item!r} to the hotbar")
        elif self._inventory.add_item(item):
            print(f"Added 1 {item!r} to the inventory")
        else:
            print(f"Found 1 {item!r}, but both hotbar & inventory are full")
            return True

        self._world.remove_item(dropped_item)
        return False

# Task 1.1 App class: Add a main function to instantiate the GUI here


def main():
    """Executes the Ninedraft class and opens the game window"""
    root = tk.Tk()
    app = Ninedraft(root)
    # display pop up message if the close button was pressed
    root.protocol("WM_DELETE_WINDOW", app._popupmsg_exit)

    root.mainloop()


if __name__ == "__main__" :
    main()

