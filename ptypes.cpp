/**********************************************************************************************
 *
 *  Particles used for the Breakout variant, by Bjørn Lindeijer
 *
 */

#include "ptypes.h"
#include "base.h"
#include "data.h"
#include "p_engine.h"

#include <cstdio>

extern Data data;

//=====   BreakoutLevel   ===================================================================//

BreakoutLevel::BreakoutLevel(BreakoutGame *imy_game, int level_nr)
    : my_game(imy_game)
{
    int brick[14][20];
    FILE *file = nullptr;

    switch (level_nr) {
    case 1:  file = std::fopen("data/level01.lev", "rb"); break;
    case 2:  file = std::fopen("data/level02.lev", "rb"); break;
    case 3:  file = std::fopen("data/goldrush.lev", "rb"); break;
    default: break;
    }

    if (file) {
        std::fread(brick, 1, sizeof(brick), file);
        std::fclose(file);
        for (int x = 0; x < 14; x++) {
            for (int y = 0; y < 20; y++) {
                if (brick[x][y] > 0)
                    level.add_particle(new Brick(this, 44 + (16 + 32 * x), 39 + (8 + 16 * y), brick[x][y]));
            }
        }
    }

    Ball *first_ball = new Ball(this, SCREEN_W / 2.f, 0, 0, 0);
    pad = new Pad((38 + 495) / 2.f, SCREEN_H - 24);
    level.add_particle(first_ball);
    level.add_particle(pad);
    pad->attach_ball(first_ball);

    // Level borders
    level.add_particle(new Block(0, 0, 38, SCREEN_H - 1)); // Left
    level.add_particle(new Block(495, 0, SCREEN_W - 1, SCREEN_H - 1)); // Right
    level.add_particle(new Block(38, 0, 495, 36)); // Top
}

void BreakoutLevel::initialize()
{
    play_sample(data.STARTUP_WAV);
}

void BreakoutLevel::update(float dt)
{
    level.update_particles(dt);
    if (nr_of_bricks == 0)
        my_game->level_finished = true;
    if (nr_of_balls == 0 && my_game->balls_left > -1) {
        my_game->balls_left--;
        if (my_game->balls_left >= 0) {
            Ball *new_ball = new Ball(this, SCREEN_W / 2.f, 0, 0, 0);
            level.add_particle(new_ball);
            pad->attach_ball(new_ball);
        }
    }
}

void BreakoutLevel::draw()
{
    level.draw_particles();
}

void BreakoutLevel::remove()
{
    level.remove_particles();
}

void BreakoutLevel::add_to_score(int points)
{
    my_game->player_score += points;
}

//=====   BreakoutGame   ====================================================================//

BreakoutGame::BreakoutGame()
    : level(new BreakoutLevel(this, curr_level))
{
}

void BreakoutGame::initialize()
{
    system->add_particle(level);
}
void BreakoutGame::update(float dt)
{
    time_played += dt;
    if (balls_left < 0) {
        // Show gameover message
        // Play gameover sound
        // Save highscore
        // Get back to main menu
    }
    if (level_finished) {
        // Show level complete message
        // Play level finished sound

        level->life = 0;
        if (curr_level == 3) {
            // Game finished message
            // Play game finished sound
            // Save highscore
            // Get back to main menu
        } else {
            curr_level++;
            // Advance to the next level
            level = new BreakoutLevel(this, curr_level);
            system->add_particle(level);
            level_finished = false;
        }
    }
}

void BreakoutGame::draw()
{
    draw_sprite(data.BORDER_BMP, 0.f, 0.f);
    draw_text(528, 40, rgb(100, 100, 100), "points");
    draw_text(528, 70, rgb(100, 100, 100), "level");
    draw_text(528, 100, rgb(100, 100, 100), "balls left");
    draw_text(528, 53, rgb(200, 100, 100), " %d", player_score);
    draw_text(528, 83, rgb(100, 200, 100), " %d", curr_level);
    draw_text(528, 113, rgb(100, 100, 200), " %d", balls_left);
}

//=====   Brick   ===========================================================================//

Brick::Brick(BreakoutLevel *imy_level, float ix, float iy, int ibrick_type)
    : my_level(imy_level)
    , brick_type(ibrick_type)
{
    type = P_BRICK;
    x = ix;
    y = iy;
    w = (data.BRICK01_BMP)->w;
    h = (data.BRICK01_BMP)->h;
    o_type = ObstacleType::Rect;
    life = 3;
    my_level->nr_of_bricks++;
}

void Brick::draw()
{
    switch (brick_type) {
    default:
        draw_rect(x - w / 2, y - h / 2, x + w / 2, y + h / 2, rgb(75, 0, 0));
        draw_line(x - w / 2, y - h / 2, x + w / 2, y + h / 2, rgb(75, 0, 0));
        draw_line(x + w / 2, y - h / 2, x - w / 2, y + h / 2, rgb(75, 0, 0));
        break;
    case 1:
        if (life == 3) {
            draw_sprite(data.BRICK01_BMP, x - w / 2, y - h / 2);
        } else {
            float alpha = max(0.f, life * 8);
            draw_sprite(data.BRICK01_BMP, x - w / 2, y - h / 2, alpha);
        }
        break;
    case 2: draw_sprite(data.BRICK02_BMP, x - w / 2, y - h / 2); break;
    case 3:
        if (life == 3)
            draw_sprite(data.BRICK03_BMP, x - w / 2, y - h / 2);
        else
            draw_sprite(data.BRICK03B_BMP, x - w / 2, y - h / 2);
        break;
    case 4:  draw_sprite(data.BRICK04_BMP, x - w / 2, y - h / 2); break;
    case 5:  draw_sprite(data.BRICK05_BMP, x - w / 2, y - h / 2); break;
    case 6:  draw_sprite(data.BRICK06_BMP, x - w / 2, y - h / 2); break;
    case 7:  draw_sprite(data.BRICK07_BMP, x - w / 2, y - h / 2); break;
    case 8:  draw_sprite(data.BRICK08_BMP, x - w / 2, y - h / 2); break;
    case 9:  draw_sprite(data.BRICK09_BMP, x - w / 2, y - h / 2); break;
    case 10: draw_sprite(data.BRICK10_BMP, x - w / 2, y - h / 2); break;
    }
}

void Brick::update(float dt)
{
    switch (brick_type) {
    case 1:
        if (life != 3)
            life -= dt;
        break;
    default: break;
    }
}

void Brick::collision(Particle *cp)
{
    if (cp->type == P_BALL) {
        switch (brick_type) {
        case 1:
            if (life == 3) {
                life = 0.125;
                play_sample(data.POP5_WAV);
            }
            break;
        case 2:
        case 3: life -= 2; break;
        case 5:
            if (life == 3) {
                life = 0;
                // Spawn the coins
                play_sample(data.BLIP1_WAV);
            }
            break;
        default: life = 0;
        }
    }
}

void Brick::remove()
{
    my_level->nr_of_bricks--;

    switch (brick_type) {
    case 1:  my_level->add_to_score(10); break;
    case 2:  my_level->add_to_score(20); break;
    case 3:  my_level->add_to_score(30); break;
    case 5:  my_level->add_to_score(100); break;
    default: break;
    }
}

//=====   Ball   ============================================================================//

Ball::Ball(BreakoutLevel *imy_level, float ix, float iy, float idx, float idy)
    : my_level(imy_level)
{
    type = P_BALL;
    x = ix;
    dx = idx;
    y = iy;
    dy = idy;
    w = h = (data.BALL01_BMP)->w;
    o_type = ObstacleType::Rect;
    affected_by_obstacle = true;
    my_level->nr_of_balls++;
}

void Ball::update(float dt)
{
    if (x - w / 2 > SCREEN_W || x + w / 2 < 0 || y - h / 2 > SCREEN_H || y + h / 2 < 0) {
        life = 0;
    }
}

void Ball::draw()
{
    draw_sprite(data.BALL01_BMP, x - w / 2, y - h / 2);
}

void Ball::collision(Particle *cp)
{
    if (cp->type == P_PAD && dy > 0) {
        float velocity = sqrt(dx * dx + dy * dy);
        float angle = atan2(dx, dy);
        angle += (x - cp->x) / 64;

        // Max angle with vertical axis is about 70 degrees
        angle = clamp(angle, -1.2f, 1.2f);

        dx = velocity * sin(angle);
        dy = velocity * cos(angle);
    }
}

void Ball::remove()
{
    my_level->nr_of_balls--;
}

//=====   Pad   =============================================================================//

Pad::Pad(float ix, float iy)
{
    type = P_PAD;
    x = ix;
    y = iy;
    w = (data.PAD01_BMP)->w;
    h = static_cast<float>((data.PAD01_BMP)->h) / 2;
    o_type = ObstacleType::Rect;
}

void Pad::update(float dt)
{
    float temp = abs(dx);
    dx = (temp - min(temp, 200 * dt)) * SGN(dx);
    if (key[KEY_LEFT])
        dx = max<float>(dx - 1200 * dt, -300);
    if (key[KEY_RIGHT])
        dx = min<float>(dx + 1200 * dt, 300);

    temp = x;
    x = max(min(x, 493 - w / 2), 39 + w / 2);
    if (x != temp)
        dx = 0;

    // On KEY_SPACE, release an attached ball
    if (key[KEY_SPACE] && !attached_balls.empty()) {
        Particle *attached_ball = attached_balls.back();
        attached_balls.pop_back();

        float speed = 300; // pixels per second
        float angle = randf() - 0.5f;
        attached_ball->dx = speed * sin(angle) + 0.75 * dx;
        attached_ball->dy = -speed * cos(angle);
    }

    // Make sure attached balls are in the right position
    for (auto &ball : attached_balls)
        ball->x = x;
}

void Pad::draw()
{
    draw_sprite(data.PAD01_BMP, x - w / 2, y - h / 2);
}

void Pad::attach_ball(Ball *the_ball)
{
    the_ball->dy = 0;
    the_ball->dx = 0;
    the_ball->y = y - h / 2 - 4;
    the_ball->x = x;
    attached_balls.push_back(the_ball);
}

//=====   Block   ===========================================================================//

Block::Block(float ix_min, float iy_min, float ix_max, float iy_max)
{
    w = ix_max - ix_min;
    h = iy_max - iy_min;
    x = (ix_min + ix_max) * 0.5f;
    y = (iy_min + iy_max) * 0.5f;
    o_type = ObstacleType::Rect;
}

void Block::draw()
{
    // draw_rect(x - w/2, y - h/2, x + w/2, y + h/2, rgb(255,0,0));
}

//=====   Stars   ===========================================================================//

Star::Star(float ix, float iy, float idx, float idy)
{
    x = ix;
    dx = idx;
    y = iy;
    dy = idy;

    int brightness;
    float initial_speed = dy;

    dy = (randf() * 0.9f + 0.1f) * dy;
    if (initial_speed != 0)
        brightness = static_cast<int>(std::clamp(255.f * (dy / initial_speed), 0.f, 255.f));
    else {
        life = 0;
        brightness = 0;
    }
    color = rgba(255, 255, 255, brightness);
}

void Star::update(float dt)
{
    if (y > SCREEN_H)
        life = 0;
}
void Star::draw()
{
    draw_point(x, y, color);
}

void StarField::initialize()
{
    for (int i = 0; i < (SCREEN_H / 25) * int(1.0 / time_per_star); i++) {
        system->add_particle(new Star(SCREEN_W * randf(), SCREEN_H * randf(), 0, 75));
    }
}

void StarField::update(float dt)
{
    time_passed += dt;
    while (time_passed > time_per_star) {
        system->add_particle(new Star(SCREEN_W * randf(), 0, 0, 75));
        time_passed -= time_per_star;
    }
}
