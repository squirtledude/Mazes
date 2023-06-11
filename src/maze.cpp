/* Your code here! */
#include "maze.h"

SquareMaze::SquareMaze() {
    // i dont allocate any new memory since all my private memebers are not pointers etc
    width_ = 0;
    height_ = 0;
}

void SquareMaze::setWall(int x, int y, int dir, bool exists) {
    // 1 = down, 0 = right
	if (dir) 
		(maze_).at(width_ * y + x).down_ = exists;
	else 
		(maze_).at(width_ * y + x).right_ = exists;
}

void SquareMaze::makeMaze(int width, int height) {
    width_ = width;
    height_ = height;

    // our disjoint set will represent all the cells initially,
    // and they will all be disjoint at first because
    // the cells will all have right and down walls
    disjoint_.addelements(width*height);

    WallData walldata_;
    walldata_.right_ = true;
    walldata_.down_ = true;

    for (int i = 0; i < width*height; ++i) {
        maze_.push_back(walldata_);
    }
    // for (int i = 0; i < width*height; ++i) {
    //     std::cout << maze_.at(i).right_ << " " << maze_.at(i).down_ << std::endl;
    // }

    // 0 for right, 1 for down (+ 1 to y coord), 2 for left, 3 for up
    for (int i = 0; i < width*height - 1; ++i) {
        std::vector<int> directions_;
        // for right logic, check if we are on the edge
        // in order to avoid cycles we will check if the next cell
        // and the current cell have the same find/parent.
        // increase i by 1 because of indexing
        if (!((i + 1) % width_ == 0) && disjoint_.find(i) != disjoint_.find(i + 1)) {
            directions_.push_back(0);
        }
        // check if up is valid, decrease height by 1 because of indexing
        if (!(width_* (height_ - 1) <= i) && disjoint_.find(i) != disjoint_.find(i+width_)) {
            directions_.push_back(1);
        }
        // for left logic, check if we are on the edge
        if (!(i % width_ == 0) && disjoint_.find(i) != disjoint_.find(i - 1)) {
            directions_.push_back(2);
        }
        // check if i is less than width because that means we cant go further up
        // we are on top row
        if (!(i < width_) && disjoint_.find(i) != disjoint_.find(i - width_)) {
            directions_.push_back(3);
        }

        // if no possible directions, then we will break out of this iteration
        // if (directions_.size() == 0) {
        //     continue;
        // }

        // using the rand function, we can get a random direction
        // using modulo so we dont get like 10000000
        int rand_direction = directions_.at(rand() % directions_.size());


        // now we will be using set union to symbolize the deletion
        // of walls.
        // to do this we will also set the bool to false
        if (rand_direction == 0) {
            disjoint_.setunion(i, i + 1);
            maze_.at(i).right_ = false;
        } else if (rand_direction == 1) {
            disjoint_.setunion(i, i + width_);
            maze_.at(i).down_ = false;
        } else if (rand_direction == 2) {
            disjoint_.setunion(i, i - 1);
            maze_.at(i - 1).right_ = false;
        } else if (rand_direction == 3) {
            disjoint_.setunion(i, i - width_);
            maze_.at(i - width_).down_ = false;
        }
    }
}

// 0 for right, 1 for down, 2 for left, 3 for up
bool SquareMaze::canTravel(int x, int y, int dir) const {
    if (dir == 0) {
        // if x is not less than the width - 1
        // that means that the x coordinate is on the x-most coord
        // so we cant go right
        if (!(x < width_ - 1)) {
            return false;
        }
        // we return the inverse of what is stored
        // since a wall is represented by true
        return !(maze_.at(x + y * width_).right_);
    } else if (dir == 1) {
        if (!(y < (height_ - 1))) {
            return false;
        }
        return !(maze_.at(x + y * width_).down_);
    } else if (dir == 2) {
        if ((x <= 0)) {
            return false;
        }
        return !(maze_.at((x - 1) + y * width_).right_);
    } else if (dir == 3) {
        if (y <= 0) {
            return false;
        }
        return !(maze_.at(x + (y - 1)*width_).down_);
    } else {
        return false;
    }
}

std::vector<int> SquareMaze::solveMaze() {
    // we need to use BFS to solve this maze
    // BFS needs some sort of structure to check if we have visited a node or not

    // this will be the solution vector. the pair will hold crucial information
    // the first element in the pair will tell us the distance traveled
    // by simplying incrementing it whenever we take a step

    // the second element will tell us which direction we went
    // for example if we go right, the right cell will now store the number 0
    // for its second element

    std::vector<std::vector<std::pair<int, int>>> traverse_;

    // I will at first make this vector store pairs of (-1, -1) 
    // since we have not traversed anything so far,
    // so we will set the distance and travel data as -1
    for (int i = 0; i < width_; ++i) {
        traverse_.push_back(std::vector<std::pair<int, int>>());
        for (int j = 0; j < height_; ++j) {
            traverse_.back().push_back(std::pair<int, int>(-1, -1));
        }
    }

    // this helper essentially takes the traverse vector and mutates it so
    // the pairs will have the correct data
    solveHelper(traverse_, std::pair<int, int>(0, 0));

    // after mutating the vector,
    // we will try to find the end cell which contains the highest value
    // which means which end cell had the longest traversal
    int biggest = traverse_.at(0).at(height_ - 1).first;
    int index = 0;

    for (int i = 0; i < width_; ++i) {
        if (traverse_.at(i).at(height_ - 1).first > biggest) {
            index = i;
            biggest = traverse_.at(i).at(height_ - 1).first;
        }
    }

    // now that we have the index we want to start our search at, lets begin working
    // backwards till we get back to (0, 0)

    std::pair<int, int> curr(index, height_ - 1);
    std::vector<int> result;
    while (curr != std::pair<int, int>(0,0)) {
        int x = curr.first;
        int y = curr.second;
        int direction = traverse_.at(x).at(y).second;
        result.push_back(direction);
        // here we will modify curr by going in the opposite direction
        // because we want to trace backwards
        // for example, if the data at curr says our direction was 0 (or right)
        // we will then go left because thats where we must have come from
        if (direction == 0) {
            curr = std::pair<int, int>(x - 1, y);
        } else if (direction == 1) {
            curr = std::pair<int, int>(x, y - 1);
        } else if (direction == 2) {
            curr = std::pair<int, int>(x + 1, y);
        } else if (direction == 3) {
            curr = std::pair<int, int>(x, y + 1);
        }
    }
    // since we had a bottom-up approach, we will reverse the vector to get the 
    // traversal from (0,0) to end cell
    std::reverse(result.begin(), result.end());
    return result;

}


void SquareMaze::solveHelper(std::vector<std::vector<std::pair<int, int>>> & traverse, std::pair<int, int> start_point){
    // this function will do the BFS work for us
    // if our solution vector says that we have visited all neighboring cells than we should be done
    // BFS could simply use a queue and get all neighboring cells
    std::queue<std::pair<int, int>> queue;
    // push the first node, which will be the origin
    queue.push(start_point);
    while (!queue.empty()) {
        std::pair<int, int> current_ = queue.front();
        int x = current_.first;
        int y = current_.second;
        queue.pop();
        std::pair<int, int> curr = traverse.at(x).at(y);
        if (canTravel(x, y, 3) && traverse.at(x).at(y - 1).first == -1) {
            traverse.at(x).at(y - 1) = std::pair<int, int>(curr.first + 1, 3);
            queue.push(std::pair<int, int>(current_.first, current_.second - 1));
        }
        if (canTravel(x, y, 2) && traverse.at(x - 1).at(y).first == -1) {
            traverse.at(x - 1).at(y) = std::pair<int, int>(curr.first + 1, 2);
            queue.push(std::pair<int, int>(current_.first - 1, current_.second));
        }
        if (canTravel(x, y, 1) && traverse.at(x).at(y + 1).first == -1) {
            traverse.at(x).at(y + 1) = std::pair<int, int>(curr.first + 1, 1);
            queue.push(std::pair<int, int>(current_.first, current_.second + 1));
        }
        if (canTravel(x, y, 0) && traverse.at(x + 1).at(y).first == -1) {
            // update traverse because we will travel to that cell
            // for right traversal, we will update the direction traveled to 0
            // since 0 symbolizes right movement

            // furthermore, we will update the first value by + 1 because we want
            // to showcase that we have increased the distance traveled by 1
            traverse.at(x + 1).at(y) = std::pair<int, int>(curr.first + 1, 0);
            queue.push(std::pair<int, int>(current_.first + 1, current_.second));
        }
    }
}

cs225::PNG* SquareMaze::drawMaze() const {
    cs225::PNG* png_ = new cs225::PNG(width_ * 10 + 1, height_ * 10 + 1);
    // now we will need to make the edge walls black

    for (int i = 0; i < width_; ++i) {
        for (int j = 0; j < height_; ++ j) {
            // this check is saying if we are on the top row,
            // then make black
            for (int additional = 0; additional < 11; ++additional) {
                if (j == 0) {
                // we need additional logic to NOT blacken the entrance
                    if (i*10 + additional < 1 || i*10 + additional > 9) {
                        cs225::HSLAPixel & curr = png_->getPixel(i*10 + additional, j);
                        curr.l = 0;
                    }  
                }
                if (i == 0) {
                    cs225::HSLAPixel & curr = png_->getPixel(i, j*10 + additional);
                    curr.l = 0;
                }
                if (!canTravel(i, j, 0)) {
                    cs225::HSLAPixel & curr = png_->getPixel((i + 1)*10, j*10 + additional);
                    curr.l = 0;
                }
                if (!canTravel(i, j, 1)) {
                    cs225::HSLAPixel & curr = png_->getPixel(i*10 + additional, (j+1)*10);
                    curr.l = 0;
                }
            }
        }
    }
    return png_;
}


cs225::PNG* SquareMaze::drawMazeWithSolution() {
    cs225::PNG* blankMaze_ = drawMaze();
    std::vector<int> solved_ = solveMaze();
    cs225::HSLAPixel & firstPoint = blankMaze_->getPixel(5, 5);

    double h_ = 0;
    double s_ = 1;
    double l_ = 0.5;
    double a_ = 1;
    firstPoint.h = h_;
    firstPoint.s = s_;
    firstPoint.l = l_;
    firstPoint.a = a_;

    int startX = 5;
    int startY = 5;
    // start at 5, 5 and then traverse according to solved_ vector
    // i totally forgot about for(auto i : vector) so i have this implementation
    // where i just keep iterating till the vector is empty
    // and when im done with one iteration, ill pop the front
    // too used to queues
    while (!solved_.empty()) {
        for (int i = 0; i < 10; ++i) {
            // whatever direction the front of the vector has
            // i will iterate in that direction
            // and change each pixel to red
            if (solved_.front() == 0) {
                cs225::HSLAPixel & curr = blankMaze_->getPixel(++startX, startY);
                curr.h = h_;
                curr.s = s_;
                curr.l = l_;
                curr.a = a_;
            } else if (solved_.front() == 1) {
                cs225::HSLAPixel & curr = blankMaze_->getPixel(startX, ++startY);
                curr.h = h_;
                curr.s = s_;
                curr.l = l_;
                curr.a = a_;
            } else if (solved_.front() == 2) {
                cs225::HSLAPixel & curr = blankMaze_->getPixel(--startX, startY);
                curr.h = h_;
                curr.s = s_;
                curr.l = l_;
                curr.a = a_;
            } else if (solved_.front() == 3) {
                cs225::HSLAPixel & curr = blankMaze_->getPixel(startX, --startY);
                curr.h = h_;
                curr.s = s_;
                curr.l = l_;
                curr.a = a_;
            }
        }
        solved_.erase(solved_.begin());
    }
    // std::cout << solved_.size() << std::endl;
    // std::cout << startX << std::endl;

    // since we are in the middle, we need to decrement the X coord by 4 to get to the start of the cell
    startX -= 4;
    // std::cout << startX << std::endl;

    // conversely, we need to INCREMENT the y coord because we are in the middle and we need to go
    // further down
    startY += 5;
    for (int lastPoint = startX; lastPoint < startX + 9; ++lastPoint) {
        cs225::HSLAPixel & curr = blankMaze_->getPixel(lastPoint, startY);
		curr.l = 1.0;
    }
    return blankMaze_;
}