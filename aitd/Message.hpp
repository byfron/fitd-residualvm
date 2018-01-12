#pragma once

namespace Msg {

enum MessageType {
	MOVE
};

class Message {
public:
	typedef std::shared_ptr<Message> Ptr;
	MessageType type;
};

class Move : public Message {
	
public:
	Move(Entity::Id id, Vec3f v) : entity(id),
								   moveVector(v) { type = MOVE; }
	
	Entity::Id entity;
	Vec3f moveVector;
};

}
