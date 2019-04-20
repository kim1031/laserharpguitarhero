import pygame
from pygame.locals import *
import time
import playsound

def doIt():
    pygame.init() 
    pygame.display.set_mode((100,100))
    starter = time.time()
    playsound.playsound("almost.mp3", False)
    print("yeee")
    
    while True: 
        event = pygame.event.get()
        if len(event) == 0:
            continue
        else:  
            event = event[0]
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_a:
                print("a DOWN " + str(time.time() - starter))
            if event.key == pygame.K_s:
                print("s DOWN " + str(time.time() - starter))
            if event.key == pygame.K_d:
                print("d DOWN " + str(time.time() - starter))
            if event.key == pygame.K_f:
                print("f DOWN " + str(time.time() - starter))
        elif event.type == pygame.KEYUP:
            if event.key == pygame.K_a:
                print("a UP " + str(time.time() - starter))
            if event.key == pygame.K_s:
                print("s UP " + str(time.time() - starter))
            if event.key == pygame.K_d:
                print("d UP " + str(time.time() - starter))
            if event.key == pygame.K_f:
                print("f UP " + str(time.time() - starter))


if __name__ == "__main__":
    doIt()