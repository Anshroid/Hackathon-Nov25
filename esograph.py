import pygame
from pygame.locals import *
from pygame import Vector2
import vidmaker
import random
import sys
import time
import math
import re

WIDTH = 1920
HEIGHT = 1200

SIMSCALE = 3

FPS = 60

WHITE = (205, 214, 244)
BLACK = (0, 0, 0)
PEACH = (250, 179, 135)
MAUVE = (203, 166, 247)
RED = (243, 139, 168)
GREEN = (166, 227, 161)
BLUE = (137, 180, 250)
SKY = (137, 220, 235)
TEAL = (148, 226, 213)
GREY = (49, 50, 68)
SURFACE1 = (69, 71, 90, 128)
YELLOW = (249, 226, 175)

pygame.init()
screen = pygame.display.set_mode((WIDTH, HEIGHT), FULLSCREEN)
pygame.display.set_caption("ESOGRAPH")
clock = pygame.time.Clock()

info = pygame.display.Info()
WIDTH = info.current_w
HEIGHT = info.current_h

SIMWIDTH = SIMSCALE * WIDTH
SIMHEIGHT = SIMSCALE * HEIGHT

CENTRE = Vector2(SIMWIDTH / 2, SIMHEIGHT / 2)

vpoffset = Vector2(SIMWIDTH / 2, SIMHEIGHT / 2) - Vector2(WIDTH / 2, HEIGHT / 2)

video = vidmaker.Video(path="vidmaker.mp4", fps=60, late_export=True)

labels = ["I", "O0", "O1", "*"]
edges = []
instructions = []

# COEFFICIENTS
g = 0.6
gdead = 10000
res = 0.05
edgecoeff = 0.0001
straighten = 0.0002
repel = 15

inp = []

with open("graph.glf", "r") as file:
    tmp = [("SWAP *", "I", "I"), ("SWAP *", "O0", "O0"), ("SWAP *", "O1", "O1"), ("SWAP *", "*", "*")]
    for line in file.read().splitlines():
        if line == "":
            continue

        if line.startswith("NODE"):
            m = re.match(r"NODE (.*)\((.*)\) (.*) (.*)", line)
            labels.append(m.group(1))
            tmp.append((m.group(2), m.group(3), m.group(4)))

        if line.startswith("CUR"):
            n = labels.index(line.split()[2])

        if line.startswith("INPUT"):
            inp = [int(x) for x in line.split()[2]]

    for i, (instr, p0, p1) in enumerate(tmp):
        edges.append((i, labels.index(p0), True))
        edges.append((i, labels.index(p1), False))

        instruction = instr.split()

        if instruction[0] == "PUSH":
            instructions.append(("PUSH", labels.index(instruction[1]), labels.index(instruction[2])))
        else:
            instructions.append(("SWAP", labels.index(instruction[1])))


I = 0
O0 = 1
O1 = 2

N = len(labels)
nodes = [CENTRE + Vector2(300 * SIMSCALE, 0).rotate(360 / (N) * i) for i in range(N)]
nodevel = [Vector2(0, 0) for i in nodes]

output = []

def step(n, inp, output):
    p0 = next(edge for edge in edges if edge[0] == n and edge[2] == True)
    p1 = next(edge for edge in edges if edge[0] == n and edge[2] == False)

    if n == I:
        x = inp.pop(0)
        n = p1[1] if x else p0[1]
        return (n, inp, output)

    elif n == O0:
        output.append(0)

    elif n == O1:
        output.append(1)

    else:
        current = instructions[n]

        xp0 = next(edge for edge in edges if edge[0] == current[1] and edge[2] == True)
        xp1 = next(edge for edge in edges if edge[0] == current[1] and edge[2] == False)
        edges.remove(xp0)
        edges.remove(xp1)

        if current[0] == "PUSH":
            edges.append((xp1[0], xp1[1], True))
            edges.append((xp1[0], current[2], False))
        
        else:
            edges.append((xp1[0], xp1[1], True))
            edges.append((xp0[0], xp0[1], False))

    newn = p0[1]
    p0 = next(edge for edge in edges if edge[0] == n and edge[2] == True)
    p1 = next(edge for edge in edges if edge[0] == n and edge[2] == False)

    return (newn, inp, output)

running = True
clicking = False
mouseHas = -1
debug = False
display = True 
recording = False
auto = False
autoCooldown = 0
animTo = None
while running:
    screen.fill((17, 17, 27))

    for event in pygame.event.get():
        if event.type == QUIT:
            running = False
        if event.type == KEYDOWN:
            if event.key == K_F4:
                if pygame.key.get_pressed()[K_LALT]:
                    running = False
            if event.key == K_SPACE:
                (n, inp, output) = step(n, inp, output)
                animTo = nodes[n] - Vector2(WIDTH/2, HEIGHT/2)
            if event.key == K_F1:
                debug = not debug
            if event.key == K_F2:
                display = not display
                pygame.display.flip()
            if event.key == K_F3:
                recording = not recording
            if event.key == K_F5:
                video.export(verbose=True)
            if event.key == K_F6:
                auto = not auto
        if event.type == MOUSEBUTTONDOWN:
            clicking = True
        if event.type == MOUSEBUTTONUP:
            clicking = False
            mouseHas = -1

    if auto:
        autoCooldown -= 1

        if autoCooldown <= 0:
            (n, inp, output) = step(n, inp, output)
            animTo = nodes[n] - Vector2(WIDTH/2, HEIGHT/2)
            autoCooldown = 10

    if animTo and SIMSCALE != 1:
        vpoffset += 1/2 * (animTo - vpoffset)
        if (animTo - vpoffset).magnitude() < 20:
            animTo = None

    if SIMSCALE != 1:
        pressed = pygame.key.get_pressed()
        if pressed[K_RIGHT]:
            vpoffset += Vector2(15, 0)
        if pressed[K_LEFT]:
            vpoffset -= Vector2(15, 0)
        if pressed[K_UP]:
            vpoffset -= Vector2(0, 15)
        if pressed[K_DOWN]:
            vpoffset += Vector2(0, 15)

    # FORCES

    for i, (pos, vel) in enumerate(zip(nodes, nodevel)):
        if i == 3:
            continue

        relpos = pos-vpoffset
        if display and (relpos.x > WIDTH or relpos.x < 0 or relpos.y > HEIGHT or relpos.y < 0):
            continue 

        accel = Vector2()
        accel += (pos - CENTRE).normalize() * (-g/math.sqrt(max((pos - CENTRE).magnitude(), gdead)) + 1/(pos - CENTRE).magnitude_squared())
        accel += -res * vel

        net = Vector2()
        dot = 0

        for edge in edges:
            if edge[0] == 3 or edge[1] == 3:
                continue

            if edge[0] == edge[1]:
                continue

            if edge[0] == i:
                net += (nodes[edge[1]] - pos)
                accel += edgecoeff * ((nodes[edge[1]] - pos).magnitude() - 200) * (nodes[edge[1]] - pos).normalize()

            elif edge[1] == i:
                net -= (pos - nodes[edge[0]])
                accel += edgecoeff * ((nodes[edge[0]] - pos).magnitude() - 200) * (nodes[edge[0]] - pos).normalize()

            # pygame.draw.aaline(screen, BLUE, pos, pos+net*0.6, width=5)

        accel += net * straighten

        for j, node in enumerate(nodes):
            if j == 3:
                continue

            if i != j:
                accel += -repel/(max((node - pos).magnitude_squared(), 100)) * (node - pos).normalize()
                pass


        nvel = vel + accel * 40
        npos = pos + vel * 40 + accel/2 * (40**2)
        
        if clicking and (mouseHas == i or mouseHas == -1) and (Vector2(pygame.mouse.get_pos()) + vpoffset - pos).magnitude() < 60:
            mouseHas = i
            npos = Vector2(pygame.mouse.get_pos()) + vpoffset
            nvel = Vector2()

        if npos.x <= 20:
            npos.x = 20
            nvel.x = 0
        elif npos.x >= SIMWIDTH - 20:
            npos.x = SIMWIDTH - 20
            nvel.x = 0

        if npos.y <= 20:
            npos.y = 20
            nvel.y = 0
        elif npos.y >= SIMHEIGHT - 20:
            npos.y = SIMHEIGHT - 20
            nvel.y = 0

        nodes[i] = npos
        nodevel[i] = nvel

    # RENDERING

    if not display:
        continue


    for edge in edges:
        col = GREEN if edge[2] else RED
        if edge[0] == 3 or edge[1] == 3:
            continue

        relpos0 = nodes[edge[0]]-vpoffset
        relpos1 = nodes[edge[1]]-vpoffset

        if relpos0.x > WIDTH or relpos0.x < 0 or relpos0.y > HEIGHT or relpos0.y < 0:
            continue 

        if relpos1.x > WIDTH or relpos1.x < 0 or relpos1.y > HEIGHT or relpos1.y < 0:
            continue 

        if edge[0] != edge[1]:
            eolvec = (nodes[edge[1]] - nodes[edge[0]]).normalize() * ((nodes[edge[1]] - nodes[edge[0]]).magnitude() - 20)
            bshift = eolvec.rotate(90).normalize()*5
            shift = bshift * (-1.5 if edge[2] else 1.5)

            pygame.draw.aaline(screen, col, relpos0+bshift, relpos1+bshift, width=5)
            pygame.draw.polygon(screen, col, [relpos0+eolvec+bshift, relpos0+(eolvec*0.9)+bshift+shift,relpos0+(eolvec*0.9)+bshift-shift])

        else:
            pygame.draw.aacircle(screen, col, relpos0+Vector2(20,20)+edge[2]*Vector2(-10,0), 20, width=5)
    
    for i, node in enumerate(nodes):
        if i == 3:
            continue

        relpos = node-vpoffset
        if relpos.x > WIDTH or relpos.x < 0 or relpos.y > HEIGHT or relpos.y < 0:
            continue 


        col = (PEACH if instructions[i][1] != 3 else GREY) if instructions[i][0] == "SWAP" else MAUVE
        if i <= 2:
            col = GREEN

        pygame.draw.aacircle(screen, col, relpos, 20)

        if i == n:
            pygame.draw.aacircle(screen, SKY, relpos, 18)
        elif i == instructions[n][1]:
            pygame.draw.aacircle(screen, RED, relpos, 18)
        elif instructions[n][0] == "PUSH" and i == instructions[n][2]:
            pygame.draw.aacircle(screen, YELLOW, relpos, 18)


        if debug:
            screen.blit(pygame.Font().render(labels[i], True, SURFACE1), relpos-Vector2(10, 10))

    screen.blit(pygame.Font().render(f"Current instruction: {instructions[n][0]} {" ".join([labels[i] for i in instructions[n][1:]])}", True, WHITE), Vector2(0,0))
    screen.blit(pygame.Font().render(f"Current input: {inp}", True, WHITE), Vector2(0,15))
    screen.blit(pygame.Font().render(f"Current output: {output}", True, WHITE), Vector2(0,30))

    if debug:
        pygame.draw.circle(screen, RED, CENTRE-vpoffset, 1)

    pygame.draw.rect(screen, SURFACE1, pygame.Rect(0 - vpoffset.x, 0 - vpoffset.y, SIMWIDTH, SIMHEIGHT), width=1)

    if recording:
        video.update(pygame.surfarray.pixels3d(screen).swapaxes(0, 1), inverted=False) # THIS LINE
    pygame.display.flip()

    # if FPS != 0:
    #     clock.tick(FPS)
    # else:
    #     time.sleep(5)

pygame.quit()