# Generate a Random Color and its invert for Image Magik convert
from random import randint

def fmt(color):
    return "#{:06x}".format(color)

mask = 0xFFFFFF
color = randint(0, mask)
invert = (~color) & mask
print("-fill {} -background {}".format(fmt(color), fmt(invert)))
