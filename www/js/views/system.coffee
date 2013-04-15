class SC.Views.System extends Backbone.View
  initialize: ->
    @template = _.template $('#system-template').html()

  events:
    "click #save-system-name": "saveSystemInfo"

  saveSystemInfo: (e) ->
    e.preventDefault()
    @model.set
      name: $("#system-name").val()
    @model.save()
    false

  render: ->
    @$el.html @template @model.toJSON()
    @el
