#include "TeaGameLib/Window/App/UpdateData.hpp"
#include "TeaGameLib/InternalGameLib/Resource/TextureResource.hpp"
#include "TeaGameLib/UtilModel/AnimSpriteModel.hpp"
#include "TeaGameLib/View.hpp"
#include "TeaGameLib/Variant.hpp" 
#include "TeaGameLib/Input.hpp"

namespace ship {

	namespace tea = teaGameLib;
	using tea::utilModel::AnimSpriteModel;
	using tea::resource::TextureResource;
	using tea::UpdateData;
	using tea::math::Vector2D;

	struct Model {
		AnimSpriteModel animSpriteModel;
		Vector2D<float>pos = { 200,200 };
		Vector2D<float>addPos = { 0,0 };
	};

	struct MsgType { 
		struct Update { float deltaTime; }; 
		struct AddPos { Vector2D<float> addPos; };
	};

	using Msg =tea::Variant<MsgType::Update,MsgType::AddPos>;


	using Cmd = tea::Cmd<Msg>;
	using Sub = tea::Sub<Msg>;

	inline UpdateData<Model, Msg> Init(std::vector<TextureResource>&& shipTextureResources) {
		return { 
			Cmd::None(),
			Model{
				AnimSpriteModel{std::move(shipTextureResources),24}
			}
		};
	}

	inline UpdateData<Model, Msg> Update(Msg msg, Model&& model) {
		ret_match(msg) ->UpdateData<Model, Msg> {
			case(msg, MsgType::Update) {
				model.animSpriteModel.Update(msg.deltaTime);
				model.pos += model.addPos * msg.deltaTime * 200;
				model.addPos = Vector2D<float>::Zero();
				return { Cmd::None() ,std::move(model) };
			}
			case(msg, MsgType::AddPos) {
				model.addPos += msg.addPos;
				return { Cmd::None() ,std::move(model) };
			}

		}match_end;
	}

	inline Sub Subscription(const Model&) {
		return Sub::Batch(
			tea::Input::KeyInput(tea::input::KeyCode::KEY_W, Msg{ MsgType::AddPos{Vector2D<float>::Up()} }),
			tea::Input::KeyInput(tea::input::KeyCode::KEY_S, Msg{ MsgType::AddPos{Vector2D<float>::Down()} }),
			tea::Input::KeyInput(tea::input::KeyCode::KEY_A, Msg{ MsgType::AddPos{Vector2D<float>::Left()} }),
			tea::Input::KeyInput(tea::input::KeyCode::KEY_D, Msg{ MsgType::AddPos{Vector2D<float>::Right()} })
		);
	};

	inline void View(const Model& model, tea::View& view) {
		view.DrawSprite(model.animSpriteModel.GetCurrentTextureResource(), model.pos.StaticCast<int>());
	}
}