import tkinter as tk


from core import get_modifiers
from crafting import GridCrafter, GridCrafterView, CraftingWindow
from block import ResourceBlock

class FurnaceBlock(ResourceBlock):
    """Furnace block for extended crafting and smelting, inherited from
    ResourceBlock class
    """
    def __init__(self, block_id, break_table):
        """Constructor

        Parameters:
        block_id (str): the string name of the block (furnace_block)
        break_table (list): the list of tools that can break or mine the item"""
        super().__init__(block_id, break_table)
        self._block_id = block_id
        self._block_id = 'furnace_block'
        self._hitpoints = 1

        # break table for furnace, only able to be mined with a pickaxe
        self._break_table = {
            'hand': (200, False),
            "wood_pickaxe": (18, True),
            "stone_pickaxe": (17, True),
            "iron_pickaxe": (15, True),
            "diamond_pickaxe": (10, True),
            "golden_pickaxe": (10, True),
            }

    def get_block_id(self):
        return self._block_id

    def get_drops(self, luck, correct_item_used):
        """Drops a itself in item form 5 times

        See Block.get_drops for parameters & return"""

        if correct_item_used:
            return [('item', (self._block_id,))]

    def use(self):
        #returns the carfting call when right clicked
        return ('crafting', 'furnace')


    def __repr__(self):
        return f"ResourceBlock({self._id!r})"



class AdvancedCraftingView(GridCrafterView):
    """New crafting window for the Furnace block, inherits from GridCrafterView"""
    def __init__(self, master, input_size):
        """Constructor:
        Parameters:
            master: passes all master information into class
            input_size: to get the correct type of crafting"""
        super().__init__(master, input_size)

        self._fire_image = tk.PhotoImage(file="flame.png")

    def render(self, key_stack_pairs, selected):
        """Renders the stacks at appropriate cells, as determined by 'key_stack_pairs'

        Parameters:
            key_stack_pairs (tuple<*, Stack>):
                    (key, stack) pairs, where each stack should be drawn at the cell
                    corresponding to key
            selected (*): The key that is currently selected, or None if no key is selected
        """

        for key, stack in key_stack_pairs:
            # print(f"Redrawing {stack} at {key}")
            if key == "output":
                # Task 2.2 Crafting: Draw output cell
                self._output.draw_cell((0,0), stack, active= key == selected)

            elif key == (1, 0):
                self._input.create_image(self._input.grid_to_xy_centre(key),
                                         image=self._fire_image)

            else:
                self._input.draw_cell(key, stack, active= key == selected)


class AdvancedCraftingWindow(CraftingWindow):
    """Creates new instance of CraftingWindow specifically for the Furnace block"""
    def _load_crafter_view(self):
        """Loads the appropriate crafter view"""
        self._source_views['crafter'] = crafter_view = \
            AdvancedCraftingView(self, self._sources['crafter'].get_input_size())

        crafter_view.pack()
        crafter_view.bind_for_id("<Button-1>", lambda key, e:
        self._handle_left_click("crafter", key, e))
        crafter_view.bind_for_id("<Button-2>", lambda key, e:
        self._handle_right_click("crafter", key, e))
        crafter_view.bind_for_id("<Button-3>", lambda key, e:
        self._handle_right_click("crafter", key, e))

    def _handle_left_click(self, widget_key, key, mouse_event):
        """Handles a left click on any cell in any widget

        Parameters:
            widget_key (str): The key of the widget clicked (e.g. 'inventory', etc.)
            key (*): The unique key of the cell in the widget that was clicked (e.g.
                     'output', (0, 0), etc.)
            mouse_event (tk.MouseEvent): The original tkinter mouse event
        """
        print(f"Left clicked on {widget_key} @ {key}")
        if key == (1,0):
            return
        selection = widget_key, key

        if selection == ('crafter', 'craft'):
            self._sources['crafter'].craft()
        else:
            self.move1(selection, get_modifiers(mouse_event.state))

        self.redraw()




