class SC.Views.AreaListItem extends Backbone.View
  tagName: 'li'

  initialize: (options) ->
    @template = _.template $('#area-list-item-template').html()
    @model.bind 'change', @render, this
    @domain = options.domain

  render: ->
    json = @model.toJSON()
    json.domain = @domain
    @$el.html @template json
    @el
