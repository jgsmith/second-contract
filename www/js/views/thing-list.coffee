class SC.Views.ThingList extends Backbone.View
  initialize: (options) ->
    @model.bind 'reset', @render, this
    @model.bind 'add', @appendNewThing, this
    @template = _.template $('#thing-list-template').html()
    @domain = @model.domain
    @area   = @model.area
    @ward   = @model.ward

  render: ->
    @$el.html @template
      domain: @domain
      area: @area
      ward: @ward
    @$ul = @$el.find("ul").first()
    for thing in @model.models
      @appendNewThing thing
    @el

  appendNewThing: (thing) ->
    item = new SC.Views.ThingListItem
      model: thing
      domain: @domain
      area: @area
      ward: @ward
    @$ul.append(item.render())
