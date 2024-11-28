#include <SFML/Graphics.hpp>
#include <iostream>
#include <forward_list>

double sign(double x) {
    if (x>=0) return 1;
    return -1;
}



unsigned short FPS = 1000;
unsigned short NUMBER_OF_OBJECTS = 2;

// physics constants
//const double G_CONSTANT = 6.6743e-11;
const double G_CONSTANT = 1;

struct objectIter {
    sf::CircleShape* curr;
    objectIter* prev;
};

struct coordinateStruct {
    double x;
    double y;
};


class Planet {
public:
    coordinateStruct coordinate;
    coordinateStruct velocity;
    coordinateStruct acceleration;

    double mass; // in kilogrammes
    sf::CircleShape shape;

    bool movable; // is movable at given system of axes

    // may be added some day
//    int radius;


    Planet(double init_x, double init_y, double init_vel_x, double init_vel_y,
           double p_mass, sf::Color color, double radius, bool p_mov) {
        coordinate.x = init_x;
        coordinate.y = init_y;

        velocity.x = init_vel_x;
        velocity.y = init_vel_y;

        acceleration = {0, 0};


        shape.setPosition(coordinate.x, coordinate.y);

        mass = p_mass;
        movable = p_mov;

        shape.setRadius(radius);
        shape.setFillColor(color);
    }

    Planet(double init_x, double init_y,
           double p_mass, sf::Color color, double radius, bool p_mov) {
        coordinate.x = init_x;
        coordinate.y = init_y;

        velocity.x = 0;
        velocity.y = 0;

        acceleration = {0, 0};


        shape.setPosition(coordinate.x, coordinate.y);

        mass = p_mass;
        movable = p_mov;

        shape.setRadius(radius);
        shape.setFillColor(color);
    }


    void move(sf::RenderWindow* window, double dx, double dy) {
        if (movable) {
            coordinate.x += dx;
            coordinate.y += dy;
            shape.setPosition(coordinate.x, coordinate.y);
        }
        window->draw(shape);
    }

    void calculate_velocity(sf::RenderWindow* window, double da_x, double da_y, double dt) {
        velocity.x += da_x * dt;
        velocity.y += da_y * dt;

        move(window, velocity.x*dt, velocity.y*dt);
    }

    void calculate_velocity(sf::RenderWindow* window, double dt) {
        velocity.x += acceleration.x * dt;
        velocity.y += acceleration.y * dt;

        move(window, velocity.x*dt, velocity.y*dt);
    }

    void calculate_acceleration(Planet* sun) {
        double r = std::sqrt(std::pow(coordinate.x - sun->coordinate.x,2) +
                             std::pow(coordinate.y - sun->coordinate.y,2));

        if (r==0) {
            std::cout<<"Singularity achieved!";
            return;
        }

        acceleration.x += G_CONSTANT * sun->mass / pow(r,3) * (sun->coordinate.x - coordinate.x);
        acceleration.y -= G_CONSTANT * sun->mass / pow(r,3) * (coordinate.y - sun->coordinate.y);
    }

    void reset_acceleration() {
        acceleration.x = 0;
        acceleration.y = 0;
    }

    // Makes an orbit of a planet round. Note that central object (sun) mustn't move in given axes
    void round_orbit(Planet* sun) {
        double r = std::sqrt(std::pow(coordinate.x - sun->coordinate.x,2) +
                std::pow(coordinate.y - sun->coordinate.y,2));

        if (r==0) {
            std::cout<<"Singularity achieved!";
            return;
        }

        double v = std::sqrt(G_CONSTANT * sun->mass / r);
        velocity.x = v * (coordinate.y - sun->coordinate.y) / r;
        velocity.y = v * (sun->coordinate.x - coordinate.x) / r;
        std::cout << "rounding " << r << ' ' << ' ' << v << velocity.y << std::endl;
    }

};


int main()
{
    auto window = sf::RenderWindow({1920, 1080}, "CMake SFML Project");
    window.setFramerateLimit(FPS);

    Planet sun((window.getSize().x / 2.f), (window.getSize().y / 2.f), 1.98e+30,
               sf::Color(255,255,0),4.f, false);

    Planet earth((window.getSize().x / 4.f), (window.getSize().y / 2.f), 0, 8e+13,  5.976e+24,
                 sf::Color(0, 0, 255), 2.f, true);

    std::cout<<(window.getSize().x / 4.f)<<std::endl;
//    earth.round_orbit(&sun);

    std::cout << earth.velocity.x<< ' '<<earth.velocity.y << std::endl;

    double t = 3e-15;
    sun.move(&window, 0,0);
    while (window.isOpen())
    {
        for (auto event = sf::Event(); window.pollEvent(event);)
        {
            if (event.type == sf::Event::Closed)
            {

                window.close();
            }

        }

        earth.calculate_acceleration(&sun);
        earth.calculate_velocity(&window, t);
        earth.reset_acceleration();

//        std::cout << earth.coordinate.x << ' ' << earth.coordinate.y << ' ' << ' ' <<
//        earth.velocity.x << ' ' << earth.velocity.y << std::endl;

//        auto shape1 = new sf::CircleShape;
//        *shape1 = shape;
//        shape1->move(0.1*t,0.1);
//        window.draw(*shape1);
//
//        delete shape1;
//        t++;

        window.display();
    }
}
