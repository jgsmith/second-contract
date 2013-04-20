class SC.Views.VerbListItem extends Backbone.View
  tagName: 'li'

  initialize: ->
    @template = _.template $('#verb-list-item-template').html()
    @model.bind 'change', @render, this

  render: ->
    @$el.html @template @model.toJSON()
    @el

