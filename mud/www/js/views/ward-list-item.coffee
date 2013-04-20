class SC.Views.WardListItem extends Backbone.View
  initialize: (options) ->
    @template = _.template $('#ward-list-item-template').html()
    @model.bind 'change', @render, this
    @domain = options.domain

  render: ->
    json = @model.toJSON()
    json.domain = @domain
    @$el.html @template json
    # now we want to list all of the items in this ward
    objectList = new SC.Views.ThingList
      model: @model.objects
    @$el.append objectList.render()
    @el
