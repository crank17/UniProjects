import random
import cmath


from mob import Mob

MOB_DEFAULT_TEMPO = 40
SHEEP_GRAVITY_FACTOR = 100
SHEEP_X_SCALE = 1.61803


class Sheep(Mob):
    """A friendly, weird, jumping sheep, that for reasons unknown is immortal and cannot be killed
    Inherited from Mob"""

    def step(self, time_delta, game_data):
        """Advance the sheep by one time step"""
        # Every 60 steps sheep moves/jumps in a random direction
        if self._steps % 60 == 0:

            health_percentage = self._health / self._max_health
            z = cmath.rect(self._tempo * health_percentage,
                           random.uniform(0, 2 * cmath.pi))

            # stretch that random point onto an ellipse that is wider on the x-axis
            dx, dy = z.real * SHEEP_X_SCALE, z.imag

            x, y = self.get_velocity()
            velocity = x + dx, y + dy - SHEEP_GRAVITY_FACTOR

            self.set_velocity(velocity)

        super().step(time_delta, game_data)
    def use(self):
        pass

    def attack(self, damage_taken):
        """when attacked, drop 5 wool"""
        return [('item', ("wool",))] * 5
