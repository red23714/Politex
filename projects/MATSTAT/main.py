import matplotlib.pyplot as plt
from csv import reader
from math import sqrt
from math import log
from random import sample


class Point:
    def __init__(self, a: float = 0, loc: float = 0):
        self.a = a
        self.loc = loc

    def get_dist(self, other):
        return sqrt((self.a - other.a) ** 2 + (self.loc + other.loc) ** 2)

    a: float
    loc: float


def find_mid(a: Point, b: Point) -> Point:
    return Point((a.a + b.a) / 2, (a.loc + b.loc) / 2)


def find_new(a: Point, b: Point, c: Point) -> Point:
    mid_point: Point = find_mid(a, b)
    return Point(2 * mid_point.a - c.a, 2 * mid_point.loc - c.loc)


def func_pravdopodobie(selection: list, p: Point) -> float:
    sum_val: float = 0
    for elem in selection:
        fxi: float = 0
        if elem - p.loc <= 1:
            fxi = log(log(p.a / 2) + (p.a - 1) * log(elem - p.loc))
        else:
            fxi = log(log(p.a / 2) - (p.a + 1) * log(elem - p.loc))

        sum_val += fxi

    return sum_val


def recurs_max_prav(
    selection: list,
    point_1: Point,
    point_2: Point,
    point_3: Point,
    fp1: float,
    fp2: float,
    fp3: float,
) -> list:
    if (
        point_1.get_dist(point_2) > 0.01
        and point_2.get_dist(point_3) > 0.01
        and point_1.get_dist(point_3) > 0.01
    ):
        return [point_1.a, point_1.loc]

    a: float = 0
    loc: float = 0

    min_fp: float = min(fp1, fp2, fp3)

    new_point: Point = Point()

    if min_fp == fp1:
        new_point = find_new(point_2, point_3, point_1)
        fp_new: float = func_pravdopodobie(selection, new_point)

        if fp_new > fp1:
            a, loc = recurs_max_prav(
                selection, new_point, point_2, point_3, fp_new, fp2, fp3
            )
        else:
            max_fp: float = max(fp1, fp2, fp3)

            if max_fp == fp2:
                new_point1: Point = find_mid(point_2, point_1)
                new_point3: Point = find_mid(point_2, point_3)

                fp_new1: float = func_pravdopodobie(selection, new_point1)
                fp_new3: float = func_pravdopodobie(selection, new_point3)
                a, loc = recurs_max_prav(
                    selection, new_point1, point_2, new_point3, fp_new1, fp2, fp_new3
                )
            else:
                new_point1: Point = find_mid(point_3, point_1)
                new_point2: Point = find_mid(point_3, point_2)

                fp_new1: float = func_pravdopodobie(selection, new_point1)
                fp_new2: float = func_pravdopodobie(selection, new_point2)
                a, loc = recurs_max_prav(
                    selection, new_point1, new_point2, point_3, fp_new1, fp_new2, fp3
                )

    elif min_fp == fp2:
        new_point = find_new(point_1, point_3, point_2)
        fp_new: float = func_pravdopodobie(selection, new_point)

        if fp_new > fp2:
            a, loc = recurs_max_prav(
                selection, point_1, new_point, point_3, fp1, fp_new, fp3
            )
        else:
            max_fp: float = max(fp1, fp2, fp3)

            if max_fp == fp1:
                new_point2: Point = find_mid(point_1, point_2)
                new_point3: Point = find_mid(point_1, point_3)

                fp_new2: float = func_pravdopodobie(selection, new_point2)
                fp_new3: float = func_pravdopodobie(selection, new_point3)
                a, loc = recurs_max_prav(
                    selection, point_1, new_point2, new_point3, fp1, fp_new2, fp_new3
                )
            else:
                new_point1: Point = find_mid(point_3, point_1)
                new_point2: Point = find_mid(point_3, point_2)

                fp_new1: float = func_pravdopodobie(selection, new_point1)
                fp_new2: float = func_pravdopodobie(selection, new_point2)
                a, loc = recurs_max_prav(
                    selection, new_point1, new_point2, point_3, fp_new1, fp_new2, fp3
                )
    else:
        new_point = find_new(point_1, point_2, point_3)
        fp_new: float = func_pravdopodobie(selection, new_point)

        if fp_new > fp3:
            a, loc = recurs_max_prav(
                selection, point_1, point_2, new_point, fp1, fp2, fp_new
            )
        else:
            max_fp: float = max(fp1, fp2, fp3)

            if max_fp == fp2:
                new_point1: Point = find_mid(point_2, point_1)
                new_point3: Point = find_mid(point_2, point_3)

                fp_new1: float = func_pravdopodobie(selection, new_point1)
                fp_new3: float = func_pravdopodobie(selection, new_point3)
                a, loc = recurs_max_prav(
                    selection, new_point1, point_2, new_point3, fp_new1, fp2, fp_new3
                )
            else:
                new_point2: Point = find_mid(point_1, point_2)
                new_point3: Point = find_mid(point_1, point_3)

                fp_new2: float = func_pravdopodobie(selection, new_point2)
                fp_new3: float = func_pravdopodobie(selection, new_point3)
                a, loc = recurs_max_prav(
                    selection, point_1, new_point2, new_point3, fp1, fp_new2, fp_new3
                )

    return [a, loc]


def find_loc_a_max_prav(selection: list) -> list:
    a, loc = find_loc_a_moment(selection)

    point_1: Point = Point(a, loc + 0.1)
    point_2: Point = Point(a, loc)
    point_3: Point = Point(a + 0.1, loc)
    fp1: float = func_pravdopodobie(selection, point_1)
    fp2: float = func_pravdopodobie(selection, point_2)
    fp3: float = func_pravdopodobie(selection, point_3)

    return recurs_max_prav(selection, point_1, point_2, point_3, fp1, fp2, fp3)
