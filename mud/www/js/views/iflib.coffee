class SC.Views.IFLib extends Backbone.View
  initialize: ->
    @template = _.template $("#iflib-template").html()

  render: ->
    @$el.html @template({})
    @el
