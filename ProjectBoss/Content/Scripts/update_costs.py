import unreal_engine as ue

class PBCosts:
    # this is called on game start
    def begin_play(self):
        ue.log('Begin Play on Hero class')

    # Called through UE4 C++. Generates a new
	def calculate_cost(self, action_name):
		print("Calculating cost for", action_name)
		return 420
		