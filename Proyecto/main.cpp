//  Materia     :   Programacion I
//  Profesor    :   Ricardo Coppo
//  Proyecto    :   "BREAK IT"
//  Alumnos     :   Luciano Cossia
//  Contacto    :   cossialuciano@gmail.com


#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cstdlib>
#include <ctime>
#include <sstream>

using namespace sf;
using namespace std;

int main()
{   
    srand((unsigned int)time(NULL));

    // Set Display

    const float width = 550.f;
    const float height = 700.f;
    VideoMode vm(550, 700);
    RenderWindow window(vm, "Break it");
    window.setFramerateLimit(60);

    // Se cargan las texturas

    Texture tBlock1, tBlock2, tBlock3, tBlock4, tBlock5, tBackground, tBall, tPad;
    tBlock1.loadFromFile("images/block01.png");
    tBlock2.loadFromFile("images/block02.png");
    tBlock3.loadFromFile("images/block03.png");
    tBlock4.loadFromFile("images/block04.png");
    tBlock5.loadFromFile("images/block05.png");
    tBackground.loadFromFile("images/background.png");
    tBall.loadFromFile("images/ball.png");
    tPad.loadFromFile("images/pad.png");

    // Creacion de los sprites

    Sprite sBackground(tBackground), sBall(tBall), sPad(tPad); // Ball 12 * 12, Pad 90 * 9 pixeles
        
    Sprite block[100];
    int n = 0;
    for (int i = 1; i <= 10; i++)
        for (int j = 1; j <= 10; j++)
        {
            if (n < 20)
                block[n].setTexture(tBlock1);
            else if (n < 40)
                block[n].setTexture(tBlock2);
            else if (n < 60)
                block[n].setTexture(tBlock3);
            else if (n < 80)
                block[n].setTexture(tBlock4);
            else
                block[n].setTexture(tBlock5);

            block[n].setPosition(j * 45.f, i * 30.f);
            n++;
        }

    // Seteo posicion de sprites
    sPad.setPosition(width / 2 - 45, height - 10);
    sBall.setPosition(width / 2 - 6, height - 25);
    
    // Preparacion del texto de inicio y score

    Text messageText, scoreText;
    Font font;
    
    font.loadFromFile("fonts/KOMIKAP_.ttf");

    messageText.setFont(font);
    scoreText.setFont(font);

    messageText.setString("Press Space to start!");
    scoreText.setString("Score = 0");

    messageText.setCharacterSize(30);
    scoreText.setCharacterSize(15);

    messageText.setFillColor(Color::White);
    scoreText.setFillColor(Color::White);

    FloatRect textRect = messageText.getLocalBounds();

    messageText.setOrigin(textRect.left + textRect.width / 2.0f,
       textRect.top + textRect.height / 2.0f);
    messageText.setPosition(width / 2.0f, height / 2.0f);
    scoreText.setPosition(5, 5);

    // Carga en buffer de sonido

    SoundBuffer Buffer;
    Buffer.loadFromFile("sounds/hit.wav");
    Sound hitsound;
    hitsound.setBuffer(Buffer);


    float speedX = 5, speedY = 5; // velocidad
    float x = width / 2.0f - 6, y = height - 25; // posicion de inicio FloatRect
    bool paused = true;
    int score = 0;
    const float padWidth = 90.f;
    const float padHeight = 9.f;
    const float ballDim = 12.f;
   
    // Procesador de eventos de la interfase gráfica

    while (window.isOpen()) {

        // Si recibimos un evento de cierre cerramos el programa
        // Si no hay eventos para procesar entonces pintamos la ventana

        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
        }

        if (Keyboard::isKeyPressed(Keyboard::Space))
            paused = false;

        if (Keyboard::isKeyPressed(Keyboard::P))
            paused = true;

        if (!paused) {

            // Se setea el Score

            stringstream contador;
            contador << "Score = " << score << " /100";
            scoreText.setString(contador.str());

            // Se incrementa la posicion de la bola --> velocidad y se crea un rectangulo de colision.
            // Cuando la bola hace contacto con el bloque, se le asigna una nueva posicion a este ultimo
            // fuera de la pantalla para que desaparezca.
            // Luego se cambia la direccion de la bola.

            x += speedX;
            for (int i = 0; i < n; i++)
                if (FloatRect(x, y, ballDim / 2.f, ballDim / 2.f).intersects(block[i].getGlobalBounds()))
                {
                    block[i].setPosition(-100, 0);
                    speedX = -speedX;
                    score++;
                    hitsound.play();
                }

            y += speedY;
            for (int i = 0; i < n; i++)
                if (FloatRect(x, y , ballDim / 2.f, ballDim / 2.f).intersects(block[i].getGlobalBounds()))
                {
                    block[i].setPosition(-100, 0);
                    speedY = -speedY;
                    score++;
                    hitsound.play();
                }             

            // Input del usuario, movimiento
            
            if (Keyboard::isKeyPressed(Keyboard::Right) && sPad.getPosition().x < width - padWidth)
                sPad.move(5, 0);

            if (Keyboard::isKeyPressed(Keyboard::Left) && sPad.getPosition().x > 0.f)
                sPad.move(-5, 0);

            if (FloatRect(x, y, ballDim, ballDim).intersects(sPad.getGlobalBounds()))
                speedY = -(float)(rand() % 5 + 1);

            // Update posicion actual

            sBall.setPosition(x, y);

            // Limite de pantalla y rebote

            if (sBall.getPosition().x < 0.f || sBall.getPosition().x > width - ballDim)
                speedX = -speedX;
            if (sBall.getPosition().y < 0.f + ballDim)
                speedY = -speedY;
            
            // Fin del Juego - Game Over

            if (sBall.getPosition().y > height) {
                paused = true;
                messageText.setString("GAME OVER");
                FloatRect loserText = messageText.getLocalBounds();
                messageText.setOrigin(loserText.left +
                    loserText.width / 2.0f,
                    loserText.top + loserText.height / 2.0f);
                messageText.setPosition(width / 2.0f, height / 2.0f);                
            }
            
            // Fin del juego - Ganador

            if (score == 100) {
                paused = true;
                messageText.setString("GANASTE!");
                FloatRect winnerText = messageText.getLocalBounds();
                messageText.setOrigin(winnerText.left +
                    winnerText.width / 2.0f,
                    winnerText.top + winnerText.height / 2.0f);
                messageText.setPosition(width / 2.0f, height / 2.0f);
            }
                
        }
        // NO hay eventos a procesar, borro la pantalla grafica    

        window.clear();

        // Dibujo los objetos indicados (en memoria)

        window.draw(sBackground);
        window.draw(sBall);
        window.draw(sPad);
        for (int i = 0; i < n; i++)
            window.draw(block[i]);
        window.draw(scoreText);
        if (paused)
            window.draw(messageText);

        // Renderizo la pantalla en el display

        window.display();
    }

    return 0;
}