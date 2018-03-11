from visual import *
from math import sin, cos, tan
from numpy.lib.scimath import logn
g = 9.80665
e = 2.71828
#Predkosc pocisku u wylotu lufy (w m/s)
predo = raw_input("Prędkość Vo pocisku u wylotu lufy: ")
predo = float(predo)
#Kąt wylotu pocisku (w stopniach)
kat = raw_input("kat: ")
kat = float(kat)
#Odleglosc miedzy dzialem a celem w metrach
odleglosc = raw_input("Odległość: ")
odleglosc = float(odleglosc)


#Masa pocisku
#masa = raw_input("Masa kulki: ")
#masa = float(masa)
#Odleglosc miedzy dzialem a celem w metrach
#opor = raw_input("Opór: ")
#opor = float(opor)


# Rozmiar okna graficznego
okno=1000
# przelicz katy ze stopni na radiany
kat = (kat/360.)*2*pi
# Dlugosc osi
L = odleglosc + 500
print """
#################################
#         Rzut ukosny           #
#################################"""
scena = display(title="Rzut ukośny", width=okno, height=okno, range=L, forward=(-1,-1,-1))
# Osie
osx = curve(pos=[(0,0,0), (L,0,0)],radius=10, color=(1.5,0.5,0.5))
osy = curve(pos=[(0,0,0), (0,L,0)],radius=10, color=(0.5,1.5,0.5))
osz = curve(pos=[(0,0,-L), (0,0,L)], color=(0.5,0.5,0.5))
# Trawa
trawa = box(pos=(0,0,0), length=odleglosc+100, height=1, width=odleglosc+5000, color=color.green)
# Punkt startowy
cel = sphere(pos=(0,0,odleglosc), radius=20, color=color.red)
# Pocisk
pocisk = sphere(pos=(0,0,0), radius=10, color=color.blue)
# Tor pocisku
pocisk.trail = curve(pos=[pocisk.pos], color=color.yellow, radius=20)
# Petla animacyjna
t = 0
while 1:
    rate(500)
    t = t + 0.005
    pocisk.pos.x = 0
    pocisk.pos.y = predo*t*sin(kat)- 0.5*g*t**2
    pocisk.pos.z = odleglosc-predo*t*cos(kat)

    #pocisk.pos.y =(predo*cos(kat)/masa*opor)*(1-e**((-opor/masa)*t))
    #pocisk.pos.z =(predo*sin(kat)+g)/(masa*opor)*(1-e**(opor/masa*t))- ((g*t)/(opor/masa))

    pocisk.trail.append(pos=pocisk.pos)

    if (pocisk.y <= 0):
        print "Pocisk wyladowal w punkcie (%f, %f, %f)" % (pocisk.pos.x, pocisk.pos.y, pocisk.pos.z)
        break # Zatrzymaj petle, aby program zakonczyl dzialanie

Z = 2*predo*predo*sin(kat)*cos(kat)/g
maxH = predo*predo*sin(kat)*sin(kat)/(2*g)

#droga = predo*predo*cos(kat)*cos(kat)/(2*g)*(logn(e, tan(kat)+sqrt(1+tan(kat)*tan(kat))+tan(kat)*sqrt(1+tan(kat))))

print "Zasięg: %f" % (Z)
print "Maksymalna wysokość: %f" % (maxH)
#print "Droga: %f" % (droga)
