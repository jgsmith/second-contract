class SC.Views.WorldLib extends Backbone.View
  initialize: ->
    @template = _.template $("#worldlib-template").html()

  render: ->
    @$el.html @template({})
    @el
