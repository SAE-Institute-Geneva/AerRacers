//
//void InputManager::BindAction()
//{
//    //TODO(@Luca) Function to bind action
//    for (unsigned i = 0; i < 4; i++) {
//        bindingPcInput_[i][static_cast<unsigned>(ActionButton::FORWARD)] =
//            static_cast<unsigned>(KeyCodeType::W);
//        bindingPcInput_[i][static_cast<unsigned>(ActionButton::BACKWARD)] =
//            static_cast<unsigned>(KeyCodeType::S);
//        bindingPcInput_[i][static_cast<unsigned>(ActionButton::RIGHT)] =
//            static_cast<unsigned>(KeyCodeType::D);
//        bindingPcInput_[i][static_cast<unsigned>(ActionButton::LEFT)] =
//            static_cast<unsigned>(KeyCodeType::A);
//        bindingPcInput_[i][static_cast<unsigned>(ActionButton::MAIN_SHOOT)] =
//            static_cast<unsigned>(KeyCodeType::E);
//        bindingPcInput_[i][static_cast<unsigned>(ActionButton::CAMERA)] =
//            static_cast<unsigned>(KeyCodeType::F);
//        bindingPcInput_[i][static_cast<unsigned>(ActionButton::MENU)] =
//            static_cast<unsigned>(KeyCodeType::ESCAPE);
//
//        bindingControllerInput_[i][static_cast<unsigned>(ActionButton::FORWARD)]
//            = PairedControllerInput(ControllerButtonType::BUTTON_A, i);
//        bindingControllerInput_[i][static_cast<unsigned>(ActionButton::BACKWARD)]
//            = PairedControllerInput(ControllerButtonType::BUTTON_A, i);
//        bindingControllerInput_[i][static_cast<unsigned>(ActionButton::RIGHT)] =
//            PairedControllerInput(ControllerButtonType::BUTTON_A, i);
//        bindingControllerInput_[i][static_cast<unsigned>(ActionButton::LEFT)] =
//            PairedControllerInput(ControllerButtonType::BUTTON_A, i);
//        bindingControllerInput_[i][static_cast<unsigned>(ActionButton::MAIN_SHOOT
//            )] =
//            PairedControllerInput(ControllerButtonType::BUTTON_A, i);
//        bindingControllerInput_[i][static_cast<unsigned>(ActionButton::CAMERA)] =
//            PairedControllerInput(ControllerButtonType::BUTTON_A, i);
//        bindingControllerInput_[i][static_cast<unsigned>(ActionButton::MENU)] =
//            PairedControllerInput(ControllerButtonType::BUTTON_A, i);
//    }
//}
//
//std::string InputManager::ActionEnumToString(const ActionButton actionInputs)
//{
//    switch (actionInputs) {
//    case ActionButton::FORWARD:
//        return "Forward";
//    case ActionButton::BACKWARD:
//        return "Backward";
//    case ActionButton::LEFT:
//        return "Left";
//    case ActionButton::RIGHT:
//        return "Right";
//    case ActionButton::MAIN_SHOOT:
//        return "Main_Shoot";
//    case ActionButton::CAMERA:
//        return "Camera";
//    case ActionButton::MENU:
//        return "Menu";
//    default:
//        return "";
//    }
//}
//
//std::string InputManager::ActionEnumToString(ActionAxis actionAxis)
//{
//    switch (actionAxis) {
//    case ActionAxis::HORIZONTAL:
//        return "Horizontal";
//    case ActionAxis::VERTICAL:
//        return "Vertical";
//    case ActionAxis::CAMERA_HORIZONTAL:
//        return "CameraHorizontal";
//    case ActionAxis::CAMERA_VERTICAL:
//        return "CameraVertical";
//    default:
//        return "";
//    }
//}