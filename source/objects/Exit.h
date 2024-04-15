#ifndef __EXIT_H__
#define __EXIT_H__

#include <cugl/cugl.h>
#include "interactable.h"

class Exit : public Interactable {
protected:
    std::string message_head_gameend;
    std::string message_head_contact;
    std::string message_head_release;

    
    /** The colors required to exit */
    std::set<std::string> _colorReqs;
    
    /** The colors that have been collected*/
    std::set<std::string> _colorsCollected;
    
    /** Time before a color is taken */
    float _ttcolor=2.0f;
    
    /** Character ref */
    std::shared_ptr<CharacterController> _character;

public:
    
    /** Amount of contact time */
    float contact_time=0.0f;
    
    /** Is in contact? */
    bool is_contacting = false;
    
    Exit(void) : Interactable() {}
    ~Exit() {
        // Call the parent class destructor
        Interactable::~Interactable();
    }
    
#pragma mark -
#pragma mark Functions
    
    const std::set<std::string> getColorReqs() const { return _colorReqs; }

    void setColorReqs(std::set<std::string> colors) { _colorReqs = colors; }
        
    const std::set<std::string> getColorsCollected() const { return _colorsCollected; }
        
    void addColor(std::string color) { _colorsCollected.insert(color); }
        
    void setColorsCollected(std::set<std::string> colors) { _colorsCollected = colors; }
        
    const std::set<std::string> getRemainingColors() const {
        std::set<std::string> result;
        std::set_difference(_colorReqs.begin(),_colorReqs.end(), _colorsCollected.begin(), _colorsCollected.end(),
            std::inserter(result, result.end()));
        return result;
    }
    
    const bool contactedLongEnough(){ return contact_time >= _ttcolor;}

    virtual bool init(const std::shared_ptr<JsonValue>& json, Vec2 scale, Rect bounds) override;

    virtual PublishedMessage onBeginContact(std::shared_ptr<cugl::physics2::Obstacle> other, b2Contact* contact = nullptr, std::shared_ptr<Interactable> otherInteractable = nullptr, bool isCharacter = false, std::shared_ptr<CharacterController> character = nullptr) override;
    
    virtual PublishedMessage timeUpdate(float timestep) override;

    virtual PublishedMessage onEndContact(std::shared_ptr<cugl::physics2::Obstacle> other, b2Contact* contact = nullptr, std::shared_ptr<Interactable> otherInteractable = nullptr, bool isCharacter = false, std::shared_ptr<CharacterController> character = nullptr) override;
    
    static std::shared_ptr<Exit> alloc(const std::shared_ptr<cugl::JsonValue>& json, Vec2 scale, Rect bounds) {
        std::shared_ptr<Exit> result = std::make_shared<Exit>();
        return (result->init(json, scale, bounds) ? result : nullptr);
    }

    
};

#endif
