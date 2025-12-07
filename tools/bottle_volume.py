from math import sqrt, pi, cos, sin

def get_radius(
        points: list[tuple[float, float]]
    ) -> float:
    """
    Finds the radius of the smallest circle encapsulating a set of points.
    """
    return max(*[sqrt(x**2 + y**2) for x, y in points])

def rotate(
        point: tuple[float, float], 
        angle: float
    ) -> tuple[float, float]:
    """
    Rotates a point around origo given a rotation angle.
    """
    x, y = point

    return (
        x * cos(angle) - y * sin(angle),
        x * sin(angle) + y * cos(angle)
    )

def scale(
        point: tuple[float, float], 
        scale: float
    ) -> tuple[float, float]:
    """
    Scales a vector.
    """
    x, y = point

    return (x * scale, y * scale)

def add(
        point1: tuple[float, float], 
        point2: tuple[float, float]
    ) -> tuple[float, float]:
    """
    Adds two vectors.
    """
    x1, y1 = point1
    x2, y2 = point2

    return (x1 + x2, y1 + y2)

def left_of_line(
        point: tuple[float, float], 
        line_point: tuple[float, float], 
        direction: tuple[float, float]
    ) -> bool:
    """
    Returns true if point is to the left of the line defined by the line point and direction.
    """
    x, y = point
    x_line, y_line = line_point
    dx, dy = direction

    return dx * (y - y_line) - dy * (x - x_line) > 0.0

def height_at_point(
        point: tuple[float, float], 
        point1: tuple[float, float], 
        point2: tuple[float, float]
    ) -> float:
    """
    Given a point returns the distance between two walls in partial cone defined by two other points.
    """
    x, y = point
    x1, y1 = point1
    x2, y2 = point2

    if y < y1 or y > y2:
        return 0.0

    if y < y1:
        return 0.0
    
    if y1 == y2 and y == y1:
        return 0.0

    k = (x2 - x1) / (y2 - y1)
    r = x1 + k * (y - y1)

    if abs(x) >= r:
        return 0.0
    
    return sqrt(r**2 - x**2) * 2.0

def approximate_volume(
        point1: tuple[float, float], 
        point2: tuple[float, float], 
        divisions: int = 128, 
        line: tuple[tuple[float, float], tuple[float, float]] = ((0.0, 0.0), (0.0, 0.0))
    ) -> float:
    """
    Approximates the volume of a partial cone defined by two points.
    """
    x1, y1 = point1
    x2, y2 = point2

    x_length = (max(x1, x2) * 2.0) / divisions
    y_length = (y2 - y1) / divisions

    y_offset = y1
    x_offset = -max(x1, x2)

    volume = 0.0

    line_point, direction = line

    for x in range(divisions):
        for y in range(divisions):
            point = (x_offset + x * x_length, y_offset + y * y_length)

            if point[1] > 1.0:
                print(point[1])
            if direction != (0.0, 0.0) and left_of_line(point, line_point, direction):
                continue

            height = height_at_point(point, point1, point2)
            volume += height * x_length * y_length

    return volume

def exact_volume(
        point1: tuple[float, float], 
        point2: tuple[float, float]
    ) -> float:
    """
    Finds the exact volume of a partial cone.
    """
    x1, y1 = point1
    x2, y2 = point2

    return 1.0 / 3.0 * pi * (y2 - y1) * (x1**2 + x2**2 + x1 * x2)

def find_y_level(
        points: list[tuple[float, float]], 
        rotation: float, 
        percentage: float, 
        divisions: int = 128,
        tolerance: float = 10e-9
    ) -> float:
    """
    Finds the y level which fills a given percentage of a volume defined by a 
    set of points and the rotation of the liquid.
    """
    _, low = points[0]
    _, high = points[-1]

    full_volume = sum([exact_volume(u, v) for u, v in zip(points, points[1:])])
    up = rotate((0.0, 1.0), -rotation)
    direction = rotate((1.0, 0.0), -rotation)
    
    while abs(high - low) > tolerance:
        mid = (high + low) / 2.0

        line_point = scale(up, mid)
        line = (line_point, direction)

        volume = sum([approximate_volume(u, v, divisions, line) for u, v in zip(points, points[1:])])

        if volume / full_volume < percentage:
            low = mid
        else:
            high = mid

    return (high + low) / 2.0

def convert_y(
        y: float, 
        points: list[tuple[float, float]]
    ) -> float:
    """
    Converts a y value to a percentage of the maximum and minimum y value.
    """
    radius = get_radius(points)

    y = max(0.0, min(1.0, y + radius))

    return y / (radius * 2.0)

def get_lookup(points: list[tuple[float, float]], num_angles: int = 256, num_ys: int = 1024, divisions: int = 128) -> list[list[float]]:
    """
    Generates a look up table for given number of angles and y levels.
    """
    lookup = [[-1.0] * num_ys for _ in range(num_angles * 4)]
    radius = get_radius(points)

    for i in range(num_ys):
        percentage = i / (num_ys - 1)

        for j in range(num_angles):
            angle = (pi / 4.0) * (j / (num_angles - 1))

            y = find_y_level(points, angle, percentage, divisions)

            if lookup[j][i] != -1.0:
                print("panic 1")
            if lookup[num_angles * 4 - j - 1][i] != -1.0:
                print("panic 2")
            if lookup[num_angles * 2 - j - 1][num_ys - i - 1] != -1.0:
                print("panic 3")
            if lookup[num_angles * 2 + j][num_ys - i - 1] != -1.0:
                print("panic 4")

            lookup[j][i] = y
            lookup[num_angles * 4 - j - 1][i] = y
            lookup[num_angles * 2 - j - 1][num_ys - i - 1] = radius - y
            lookup[num_angles * 2 + j][num_ys - i - 1] = radius - y


if __name__ == "__main__":
    points: list[tuple[float, float]] = [
        (0.0304, -0.1129),
        (0.0304, 0.0747),
        (0.0131, 0.1097),
        (0.0131, 0.1733)
    ]
    
    exact = sum([exact_volume(u, v) for u, v in zip(points, points[1:])])
    angle = pi / 4
    y = find_y_level(points, angle, 0.01)
    p = rotate((0.0, y), angle)
    d = rotate((1.0, 0.0), angle)
    line = (p, d)
    approximate = sum([approximate_volume(u, v, 128, line) for u, v in zip(points, points[1:])])

    get_lookup(points, 4, 16)

    print("Exact:      ", exact)
    print("Approximate:", approximate)
    print("Ratio:      ", approximate / exact)
    print("Error:      ", abs(exact - approximate))