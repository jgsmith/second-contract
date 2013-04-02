SC.Views.System = Backbone.View.extend
  initialize: ->
    this.template = _.template $('#system-template').html()

  events:
    "click #save-system-name": "saveSystemInfo"

  saveSystemInfo: (e) ->
    e.preventDefault()
    this.model.set
      name: $("#system-name").val()
    this.model.save()
    false

  render: ->
    this.$el.html this.template this.model.toJSON()
    this.el
