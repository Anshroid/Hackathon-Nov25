# ESOGRAPH!
-# by Zhanqi Wang, Anshul Nema

https://github.com/user-attachments/assets/28dda6c3-3aeb-468e-ae62-15e71af29176

Welcome to Esograph, the unnecessarily confusing DFA in disguise (probably) that we (nearly) made a brainfuck compiler for!

Esograph is a self-modifying language: it runs on a graph that makes changes to itself as the program runs.

The graph is described as:
- A set of nodes, in addition to a few special nodes - $I, O_0, O_1, \*$ where $I$ is the node that will read input, $O_0$ and $O_1$ will write output and $\*$ is an arbitrary node that simply points to itself and so is used as a garbage/halting state.
- Each node is attached to two other nodes. These two branches are denoted $p_0$ and $p_1$. $p_0$ is the branch the language will always take by default.
- Each node then has an associated instruction, which is either PUSH or SWAP. I'll explain these in a bit:

We present the graph graphically using a force-directed method and a specific colour code. Peach is a SWAP node, mauve is a PUSH node and grey is a NOP node (which is SWAP * * in disguise).
Green nodes are I/O, and it's usually pretty obvious whether it's an input or an output based on whether there are arrows going in or out.
The cyan filled node is the currently executing node, the red filled node is the target of the next operation and the yellow filled node is the secondary target for the PUSH operation.

## SWAP
The SWAP instruction, as the name suggests, will swap $p_0$ and $p_1$ on a certain node. You can see this in the following demonstration:

https://github.com/user-attachments/assets/c5f4b570-319f-4670-9507-45684bcac35d

Each time the peach node is reached, it swaps the branches of the node to the lower right of it. At the end of this demonstration the machine reaches an infinite loop.
From now on we'll suppress most of the instances where a node links to itself. You can assume that the node points to $\*$ if there's no arrow coming out of a certain colour.

## PUSH
The PUSH instruction is a little more complicated. It sets $p_0(x) := p_1(x)$ and then $p_1(x) := y$ for some provided $y$. This essentially updates the branches coming out of a given node $x$, pointing it to a new node $y$

https://github.com/user-attachments/assets/a6ebfda2-e391-408f-b34d-69d14760111c

## Bits
We can combine these two very simple (or very complicateed, depending on how you look at it) instructions to make a representation of a bit. Each bit is made up of 6 nodes:
- b -> bp0 and bp1. These represent the bit itself - if the bit is set to 0 then when the graph passes through b it will follow the bp0 branch. If the bit is set to 1 then when the graph passes through b it will follow the bp1 branch.
- f. This flips the bit using a SWAP instruction.
- r0 -> r1. These reset the bit to 0 using two PUSH instructions.

You can see these in action in this demo:

https://github.com/user-attachments/assets/66d2d109-fd8e-47af-af96-8683bcc12a92

From here on, GitHub won't let me upload the files to the markdown directly. I'll attach an unlisted youtube video with the remaining clips: https://www.youtube.com/watch?v=9eSy7x4UbUg

## Logic gates
Here is a demo of an XOR gate. This is using a "compiler" you can find in codegen.cpp, which provides abstractions of each of the important gates and such. First we have an input sequence using rx and ry, where the I node redirects either to set x to 0 or flip x to 1, and the same for the bit y.

Then we go through the XOR sequence, which uses the property that `xor 1 y` = `not (xor 0 y)` by hardcoding the values of `xor 0 y` using `y_bp` 0 and 1 and simply flipping some of the gates to get the values for `xor 1 y`.

Finally we go through the output sequence, which reads the bit containing the output of the XOR and branches through $O_0$ or $O_1$, outputing the result.

## Half adder
Oh boy, it only gets more complicated from here. We make an AND gate similarly to the XOR gate, and we make a "Duplication Gate" that copies the value stored in a bit into another bit. Then we chain these together to make a half adder. You can vaguely see the shapes of the various gates:

## Byte negater
The simulation is starting to lag, and it almost doesn't fit on one screen. This one chains 8 of the bit gates together to make a byte gate. We're so close to a brainfuck compiler...

## Byte increment
I had to increase the simulation bounds by 3x to fit this one. It has over 600 nodes and I had to do so many simulation optimisations to try to make this work in a reasonable amount of time...

## Brainfuck compiler
We almost got it! The only thing left to make this work was simulating the tape. We obviously couldn't use an infinite tape, but even with a finite tape our bit system is so clunky that it's really difficult to chain anything together at all.

## Remarks
Honestly, this was a pain to make. Both trying to make all the logic work, and trying to make the simulation work, and staying up all night, and trying to do it all in time. I'm sure something could come out of it, but I think it looks cool at least...



