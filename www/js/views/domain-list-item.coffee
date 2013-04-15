class SC.Views.DomainListItem extends Backbone.View
  tagName: 'li'

  initialize: ->
    @template = _.template $('#domain-list-item-template').html()
    @model.bind 'change', @render, this

  render: ->
    @$el.html @template @model.toJSON()
    @el
