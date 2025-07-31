#pragma once
enum class AIAction : int {
    MOVE_NORTH = 0, MOVE_SOUTH = 1, MOVE_WEST = 2, MOVE_EAST = 3,
    MOVE_NORTHEAST = 4, MOVE_NORTHWEST = 5, MOVE_SOUTHEAST = 6, MOVE_SOUTHWEST = 7,
    MOVE_TO_ENEMY = 8, ATTACK_TARGET = 9,
    SKILL_Q = 10, SKILL_E = 11, SKILL_R = 12,
    RETREAT_SAFE = 13, STAY_POSITION = 14,
    FOLLOW_NAVIGATION = 15, CANCEL_NAVIGATION = 16,
    TOTAL_ACTIONS = 17
};
struct ActionResult {
    bool success;
    AIAction attempted_action;
    std::string failure_reason;
    float penalty_reward;  // 실패에 대한 페널티
   
};