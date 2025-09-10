import pygame

pygame.mixer.init()
pygame.mixer.music.set_volume(1)
print("playing sound")
pygame.mixer.music.load("/home/pi/Scripts/jailalarm.mp3")
pygame.mixer.music.play(-1)