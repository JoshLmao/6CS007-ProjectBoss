import unreal_engine as ue

class PBCosts:

	def begin_play(self):
		print("Name", self.uobject.get_name())
		#self.uobject.call("UpdateActionCost action-name 420")

	def calculate_cost(self, action_name):
		print("Calculating cost for", action_name)
		return 420
		