CC=g++ 
LF=-framework OpenGL -framework IOKit -framework Cocoa -lglad -lglfw3 -lfreetype

# game.cpp object.cpp shader.cpp player.cpp coin.cpp laser.cpp coinbox.cpp laserbox.cpp textobject.cpp 

game: shader object player coin laser coinbox laserbox textobject
	$(CC) object.o shader.o player.o coin.o laser.o coinbox.o laserbox.o textobject.o $(LF)

shader:
	$(CC) shader.cpp -c 

object:
	$(CC) object.cpp -c

player:
	$(CC) player.cpp -c

coin:
	$(CC) coin.cpp -c

laser:
	$(CC) laser.cpp -c

coinbox:
	$(CC) coinbox.cpp -c

laserbox:
	$(CC) laserbox.cpp -c

textobject:
	$(CC) textobject.cpp -c

