#pragma once

#include <variant>

/**
 * An explanation about the purpose of this instance.
 */
struct MessageElement {
    /**
     * An explanation about the purpose of this instance.
     */
    int64_t device_id;
    /**
     * An explanation about the purpose of this instance.
     */
    int64_t max;
    /**
     * An explanation about the purpose of this instance.
     */
    int64_t min;
    /**
     * An explanation about the purpose of this instance.
     */
    int64_t parameter_number;
    /**
     * An explanation about the purpose of this instance.
     */
    std::string type;
};

/**
 * An explanation about the purpose of this instance.
 */
struct ValueElement {
    /**
     * An explanation about the purpose of this instance.
     */
    std::string id;
    /**
     * An explanation about the purpose of this instance.
     */
    int64_t max;
    /**
     * An explanation about the purpose of this instance.
     */
    MessageElement message;
    /**
     * An explanation about the purpose of this instance.
     */
    int64_t min;
};

/**
 * An explanation about the purpose of this instance.
 */
struct ControlElement {
    /**
     * An explanation about the purpose of this instance.
     */
    std::vector<int64_t> bounds;
    /**
     * An explanation about the purpose of this instance.
     */
    std::string color;
    /**
     * An explanation about the purpose of this instance.
     */
    int64_t control_set_id;
    /**
     * An explanation about the purpose of this instance.
     */
    int64_t id;
    /**
     * An explanation about the purpose of this instance.
     */
    std::string name;
    /**
     * An explanation about the purpose of this instance.
     */
    int64_t page_id;
    /**
     * An explanation about the purpose of this instance.
     */
    int64_t pot_id;
    /**
     * An explanation about the purpose of this instance.
     */
    std::string type;
    /**
     * An explanation about the purpose of this instance.
     */
    std::vector<ValueElement> values;
};

/**
 * An explanation about the purpose of this instance.
 */
struct DeviceElement {
    /**
     * An explanation about the purpose of this instance.
     */
    int64_t channel;
    /**
     * An explanation about the purpose of this instance.
     */
    int64_t id;
    /**
     * An explanation about the purpose of this instance.
     */
    std::string name;
    /**
     * An explanation about the purpose of this instance.
     */
    int64_t port;
};

/**
 * An explanation about the purpose of this instance.
 */
struct GroupElement {
    /**
     * An explanation about the purpose of this instance.
     */
    std::vector<int64_t> bounds;
    /**
     * An explanation about the purpose of this instance.
     */
    std::string color;
    /**
     * An explanation about the purpose of this instance.
     */
    std::string name;
    /**
     * An explanation about the purpose of this instance.
     */
    int64_t page_id;
};

/**
 * An explanation about the purpose of this instance.
 */
struct ItemElement {
    /**
     * An explanation about the purpose of this instance.
     */
    std::string label;
    /**
     * An explanation about the purpose of this instance.
     */
    int64_t value;
};

/**
 * An explanation about the purpose of this instance.
 */
struct OverlayElement {
    /**
     * An explanation about the purpose of this instance.
     */
    int64_t id;
    /**
     * An explanation about the purpose of this instance.
     */
    std::vector<ItemElement> items;
};

/**
 * A page associate a a page name with an identifier. The page identifier is then used in
 * other objects to include them on a specific page.
 */
struct PageElement {
    /**
     * A unique identifier of the page. The identifier actually represent particular page within
     * the array of 12 pages that Electra One can display.
     */
    int64_t id;
    /**
     * A name of the page as it will be shown to the user on the status bar and on the page
     * selection screen.
     */
    std::string name;
};

struct CoordinateClass {
    /**
     * An explanation about the purpose of this instance.
     */
    std::vector<ControlElement> controls;
    /**
     * An explanation about the purpose of this instance.
     */
    std::vector<DeviceElement> devices;
    /**
     * An explanation about the purpose of this instance.
     */
    std::shared_ptr<std::vector<GroupElement>> groups;
    /**
     * A name of the preset as it will be shown on Electra's status bar.
     */
    std::string name;
    /**
     * An explanation about the purpose of this instance.
     */
    std::shared_ptr<std::vector<OverlayElement>> overlays;
    /**
     * A list of individual pages of the preset. A page is a collection of controls that are
     * shown on the screen at one moment.
     */
    std::vector<PageElement> pages;
    /**
     * Identifier used by the Electra One editor to associate preset with an Editor project.
     */
    std::shared_ptr<std::string> project_id;
    /**
     * A numeric representation of the preset file format.
     */
    int64_t version;
};
